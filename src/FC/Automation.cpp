#include "Automation.h"

namespace Automation {
    Task *flowTask = nullptr;
    Task *abortFlowTask = nullptr;
    Task *checkForAbortTask = nullptr;

    float loxLead = 0.165;
    float burnTime = 22.0; //22.0

    bool igniterEnabled = true;
    bool breakwireEnabled = true;
    bool thrustEnabled = false;

    bool igniterTriggered = false;

    float loadCellValue;

    void initAutomation(Task *flowTask, Task *abortFlowTask, Task *checkForAbortTask) {
        Automation::flowTask = flowTask;
        Automation::abortFlowTask = abortFlowTask;
        Automation::checkForAbortTask = checkForAbortTask;

        Comms::registerCallback(150, beginFlow);
        Comms::registerCallback(151, beginAbortFlow);
        Comms::registerCallback(120, readLoadCell);
    }

    Comms::Packet flowPacket = {.id = 50};
    int step = 0;

    void beginFlow(Comms::Packet packet) {
        if(!flowTask->enabled) {
            step = 0;
            igniterTriggered = false;
            flowTask->nexttime = micros();
            flowTask->enabled = true;
        }
    }
    inline void sendFlowStatus(uint8_t status) {
        DEBUG("FLOW STATUS: ");
        DEBUG(status);
        DEBUG("\n");
        flowPacket.len = 1;
        flowPacket.data[0] = status;
        Comms::emitPacket(&flowPacket);
    }
    uint32_t flow() {
        DEBUG("STEP: ");
        DEBUG(step);
        DEBUG("\n");
        switch(step) {
            case 0: // step 0 (actuate igniter)
                if(Valves::breakWire.voltage > breakWireThreshold || !breakwireEnabled) {
                    Valves::activateIgniter();
                    sendFlowStatus(0);
                    step++;
                    return 2000 * 1000; // delay 2s
                } else {
                    sendFlowStatus(14);
                    beginAbortFlow();
                    return 0;
                }
            case 1: // step 1 
                // check igniter current trigger and break wire
                if ((igniterTriggered || !igniterEnabled)
                        && (Valves::breakWire.voltage < breakWireThreshold || !breakwireEnabled)) {
                    Valves::deactivateIgniter();
                    Valves::openArmValve();
                    sendFlowStatus(1);
                    step++;
                    return 500 * 1000; // delay 0.5s
                } else {
                    sendFlowStatus(15);
                    beginAbortFlow();
                    return 0;
                }
            case 2: // step 2
                // check arm valve current, main valve continuity else abort
                if (Valves::armValve.current > currentThreshold) {
                    Valves::openLoxMainValve();
                    sendFlowStatus(2);
                    step++;
                    return loxLead * 1000000; // delay by lox lead
                } else {
                    sendFlowStatus(16);
                    beginAbortFlow();
                    return 0;
                }
            case 3: // step 3
                // check arm valve current, loxMain current, fuelMain continuity
                if (Valves::armValve.current > currentThreshold
                        && Valves::loxMainValve.current > currentThreshold) {
                    Valves::openFuelMainValve();
                    //begin checking thermocouple values
                    // checkForAbortTask->enabled = true;
                    sendFlowStatus(3);
                    checkForAbortTask->enabled = true;
                    step++;
                    return (burnTime / 2) * 1000000; // delay by burn time
                } else {
                    sendFlowStatus(17);
                    beginAbortFlow();
                    return 0;
                }
            case 4:
                sendFlowStatus(4);
                step++;
                return (burnTime / 2) * 1000000;
            case 5: // step 5 (close fuel)
                Valves::closeFuelMainValve();
                checkForAbortTask->enabled = false;
                sendFlowStatus(5);
                step++;
                return 200 * 1000; // delay by burn time
            case 6: // step 6 (close lox)
                Valves::closeLoxMainValve();
                sendFlowStatus(6);
                step++;
                return 500 * 1000; // delay by burn time
            case 7: // step 7 (close arm valve)
                Valves::closeArmValve();
                sendFlowStatus(7);
                step++;
                return 0; // delay by burn time
            default: // end
                flowTask->enabled = false;
                sendFlowStatus(8);
                return 0;
        }
    }

    void beginAbortFlow(Comms::Packet packet) {
        beginAbortFlow();
    }

    void beginAbortFlow() {
        sendFlowStatus(9); //abort status
        if(!abortFlowTask->enabled) {
            flowTask->enabled = false;
            abortFlowTask->nexttime = micros();
            abortFlowTask->enabled = true;
            checkForAbortTask->enabled = false;
        }
    }

    uint32_t abortFlow() {
        Valves::closeFuelMainValve();
        Valves::closeLoxMainValve();
        Valves::closeArmValve();
        Valves::deactivateIgniter();
        abortFlowTask->enabled = false;
        sendFlowStatus(10);
        return 0;
    }

    uint32_t checkIgniter() {
        igniterTriggered = Valves::igniter.current > igniterTriggerThreshold || igniterTriggered;
        return 50 * 1000; //TODO determine appropriate sampling time
    }

    void readLoadCell(Comms::Packet packet) {
        float loadCell1Value = Comms::packetGetFloat(&packet, 0);
        float loadCell2Value = Comms::packetGetFloat(&packet, 4);
        float loadCellSum = Comms::packetGetFloat(&packet, 8);

        loadCellValue = loadCellSum;
    }

    uint32_t checkForAbort() {
        //check thermocouple temperatures to be below a threshold
        float maxThermocoupleValue = max(max(Thermocouples::engineTC0Value, Thermocouples::engineTC1Value), 
                                        max(Thermocouples::engineTC2Value, Thermocouples::engineTC3Value));

        DEBUG("LOAD CELL VALUE: ");
        DEBUG(loadCellValue);
        DEBUG(" TC VALUE: ");
        DEBUG(Thermocouples::engineTC1Value);
        DEBUG(" TC ROC: ");
        DEBUG(Thermocouples::engineTC1ROC);
        DEBUG("\n");

        if (maxThermocoupleValue > thermocoupleAbsoluteThreshold) {
            sendFlowStatus(11);
            beginAbortFlow();
        }

        if (Thermocouples::engineTC0Value > thermocoupleThreshold && Thermocouples::engineTC0ROC > thermocoupleRateThreshold) {
            sendFlowStatus(12);
            beginAbortFlow();
        }

        if (Thermocouples::engineTC1Value > thermocoupleThreshold && Thermocouples::engineTC1ROC > thermocoupleRateThreshold) {
            sendFlowStatus(12);
            beginAbortFlow();
        }
        if (Thermocouples::engineTC2Value > thermocoupleThreshold && Thermocouples::engineTC2ROC > thermocoupleRateThreshold) {
            sendFlowStatus(12);
            beginAbortFlow();
        }
        if (Thermocouples::engineTC3Value > thermocoupleThreshold && Thermocouples::engineTC3ROC > thermocoupleRateThreshold) {
            sendFlowStatus(12);
            beginAbortFlow();
        }

        if (step == 5) {
            if (loadCellValue < loadCellThreshold && thrustEnabled) {
                sendFlowStatus(13);
                beginAbortFlow();
            }
        }

        return 100 * 1000; //TODO determine appropriate sampling time
    }
};
