#include "Automation.h"

namespace Automation {
    Task *flowTask = nullptr;
    Task *abortFlowTask = nullptr;
    Task *checkForAbortTask = nullptr;

    float loxLead = 0.165;
    float burnTime = 300.0; //3.0

    bool igniterEnabled = false;
    bool breakwireEnabled = false;

    bool igniterTriggered = false;

    float loadCellValue;

    void initAutomation(Task *flowTask) {
        Automation::flowTask = flowTask;
        Comms::registerCallback(150, beginFlow);
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
                Valves::activateIgniter();
                sendFlowStatus(0);
                step++;
                return 2000 * 1000; // delay 2s
            default: // end
                Valves::deactivateIgniter();
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
            beginAbortFlow();
        }

        if (Thermocouples::engineTC0Value > thermocoupleThreshold && Thermocouples::engineTC0ROC > thermocoupleRateThreshold) {
            beginAbortFlow();
        }

        if (Thermocouples::engineTC1Value > thermocoupleThreshold && Thermocouples::engineTC1ROC > thermocoupleRateThreshold) {
            beginAbortFlow();
        }
        if (Thermocouples::engineTC2Value > thermocoupleThreshold && Thermocouples::engineTC2ROC > thermocoupleRateThreshold) {
            beginAbortFlow();
        }
        if (Thermocouples::engineTC3Value > thermocoupleThreshold && Thermocouples::engineTC3ROC > thermocoupleRateThreshold) {
            beginAbortFlow();
        }

        if (step == 5) {
            if (loadCellValue < loadCellThreshold) {
                beginAbortFlow();
            }
        }

        return 100 * 1000; //TODO determine appropriate sampling time
    }
};
