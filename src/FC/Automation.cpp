#include "Automation.h"

namespace Automation {
    Task *flowTask = nullptr;
    Task *abortFlowTask = nullptr;
    Task *checkForAbortTask = nullptr;

    uint32_t loxLead = 165 * 1000;
    uint32_t burnTime = 22 * 1000 * 1000; //22.0 (total burntime - 2)

    bool igniterEnabled = false;
    bool breakwireEnabled = false;
    bool thrustEnabled = false;

    bool igniterTriggered = false;

    float loadCellValue;

    //each index maps to a check
    uint8_t hysteresisValues[6] = {0};
    uint8_t hysteresisThreshold = 10;

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
            //reset values
            igniterTriggered = false;
            flowTask->nexttime = micros();
            flowTask->enabled = true;
            checkForAbortTask->enabled = false;
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
                    return loxLead; // delay by lox lead
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
                    checkForAbortTask->enabled = true;
                    sendFlowStatus(3);
                    step++;
                    return 2 * 1000 * 1000; // delay by 2 seconds
                } else {
                    sendFlowStatus(17);
                    beginAbortFlow();
                    return 0;
                }
            case 4:
                DEBUG((uint32_t)((burnTime - 2) * 1e6));
                DEBUG('\n');
                sendFlowStatus(4);
                step++;
                return burnTime - (2 * 1000 * 1000); //delay by burn time - 2 seconds
            case 5: // step 5 (close fuel)
                Valves::closeFuelMainValve();
                checkForAbortTask->enabled = false;
                sendFlowStatus(5);
                step++;
                return 200 * 1000;
            case 6: // step 6 (close lox)
                Valves::closeLoxMainValve();
                sendFlowStatus(6);
                step++;
                return 500 * 1000;
            case 7: // step 7 (close arm valve)
                Valves::closeArmValve();
                sendFlowStatus(7);
                step++;
                return 1 * 1000 * 1000; //delay for gap in status messages
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
        DEBUG(Thermocouples::engineTC3Value);
        DEBUG(" TC ROC: ");
        DEBUG(Thermocouples::engineTC3ROC);
        DEBUG(" Hysteresis TC3: ");
        DEBUG(hysteresisValues[4]);
        DEBUG("\n");

        if (maxThermocoupleValue > thermocoupleAbsoluteThreshold) {
            hysteresisValues[0] += 1;
            if (hysteresisValues[0] >= hysteresisThreshold) {
                sendFlowStatus(11);
                beginAbortFlow();
            }
        } else {
            hysteresisValues[0] = 0;
        }

        if (Thermocouples::engineTC0Value > thermocoupleThreshold && Thermocouples::engineTC0ROC > thermocoupleRateThreshold) {
            hysteresisValues[1] += 1;
            if (hysteresisValues[1] >= hysteresisThreshold) {
                sendFlowStatus(12);
                beginAbortFlow();
            }
        } else {
            hysteresisValues[1] = 0;
        }

        if (Thermocouples::engineTC1Value > thermocoupleThreshold && Thermocouples::engineTC1ROC > thermocoupleRateThreshold) {
            hysteresisValues[2] += 1;
            if (hysteresisValues[2] >= hysteresisThreshold) {
                sendFlowStatus(12);
                beginAbortFlow();
            }
        } else {
            hysteresisValues[2] = 0;
        }

        if (Thermocouples::engineTC2Value > thermocoupleThreshold && Thermocouples::engineTC2ROC > thermocoupleRateThreshold) {
            hysteresisValues[3] += 1;
            if (hysteresisValues[3] >= hysteresisThreshold) {
                sendFlowStatus(12);
                beginAbortFlow();
            }
        } else {
            hysteresisValues[3] = 0;
        }

        if (Thermocouples::engineTC3Value > thermocoupleThreshold && Thermocouples::engineTC3ROC > thermocoupleRateThreshold) {
            hysteresisValues[4] += 1;
            if (hysteresisValues[4] >= hysteresisThreshold) {
                sendFlowStatus(12);
                beginAbortFlow();
            }
        } else {
            hysteresisValues[4] = 0;
        }

        if (step == 5) {
            if (loadCellValue < loadCellThreshold && thrustEnabled) {
                hysteresisValues[5] += 1;
                if (hysteresisValues[5] >= hysteresisThreshold) {
                    sendFlowStatus(13);
                    beginAbortFlow();
                }
            } else {
                hysteresisValues[5] = 0;
            }
        }

        return 100 * 1000; //TODO determine appropriate sampling time
    }
};
