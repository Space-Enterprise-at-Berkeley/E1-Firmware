#include "Automation.h"

namespace Automation {
    Task *flowTask = nullptr;
    Task *abortFlowTask = nullptr;

    float loxLead = 0.165;
    float burnTime = 3.0;

    bool igniterEnabled = false;
    bool breakwireEnabled = false;

    bool igniterTriggered = false;

    void initAutomation() {
        Comms::registerCallback(150, beginFlow);
        Comms::registerCallback(151, beginAbortFlow);
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
        switch(step) {
            case 0: // step 0 (actuate igniter)
                if(Valves::breakWire.voltage > breakWireThreshold || !breakwireEnabled) {
                    Valves::activateIgniter();
                    sendFlowStatus(0);
                    step++;
                    return 2000 * 1000; // delay 2s
                } else {
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
                    beginAbortFlow();
                    return 0;
                }
            case 3: // step 3
                // check arm valve current, loxMain current, fuelMain continuity
                if (Valves::armValve.current > currentThreshold
                        && Valves::loxMainValve.current > currentThreshold) {
                    Valves::openFuelMainValve();
                    sendFlowStatus(3);
                    step++;
                    return burnTime * 1000000; // delay by burn time
                } else {
                    beginAbortFlow();
                    return 0;
                }
            case 4: // step 4 (close fuel)
                Valves::closeFuelMainValve();
                sendFlowStatus(4);
                step++;
                return 200 * 1000; // delay by burn time
            case 5: // step 5 (close lox)
                Valves::closeLoxMainValve();
                sendFlowStatus(5);
                step++;
                return 500 * 1000; // delay by burn time
            case 6: // step 6 (close arm valve)
                Valves::closeArmValve();
                sendFlowStatus(6);
                step++;
                return 0; // delay by burn time
            default: // end
                flowTask->enabled = false;
                sendFlowStatus(7);
                return 0;
        }
    }

    void beginAbortFlow(Comms::Packet packet) {
        beginAbortFlow();
    }

    void beginAbortFlow() {
        sendFlowStatus(8); //abort status
        if(!abortFlowTask->enabled) {
            flowTask->enabled = false;
            abortFlowTask->nexttime = micros();
            abortFlowTask->enabled = true;
        }
    }

    uint32_t abortFlow() {
        Valves::closeFuelMainValve();
        Valves::closeLoxMainValve();
        Valves::closeArmValve();
        Valves::deactivateIgniter();
        abortFlowTask->enabled = false;
        sendFlowStatus(9);
        return 0;
    }

    uint32_t checkIgniter() {
        igniterTriggered = Valves::igniter.current > igniterTriggerThreshold || igniterTriggered;
        return 50 * 1000; //TODO determine appropriate sampling time
    }
};
