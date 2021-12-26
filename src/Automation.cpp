#include <Automation.h>

namespace Automation {
    Task *flowTask = nullptr;

    float loxLead = 0.165;
    float burnTime = 3.0;

    void initAutomation() {}

    Comms::Packet flowPacket = {.id = 50};
    uint8_t step = 0;
    void beginFlow() {
        if(!flowTask->enabled) {
            step = 0;
            flowTask->nexttime = micros();
            flowTask->enabled = true;
        }
    }
    inline void sendFlowStatus(uint8_t status) {
        flowPacket.len = 1;
        flowPacket.data[0] = status;
        Comms::emitPacket(&flowPacket);
    }
    uint32_t flow() {
        switch(step) {
            case 0: // step 0 (actuate igniter)
                Valves::activateIgniter();
                sendFlowStatus(0);
                step++;
                return 2000 * 1000; // delay 2s
            case 1: // step 1 
                // check igniter current trigger and break wire
                Valves::deactivateIgniter();
                Valves::openArmValve();
                sendFlowStatus(1);
                step++;
                return 500 * 1000; // delay 0.5s
            case 2: // step 2
                // check arm valve current, main valve continuity
                Valves::openLoxMainValve();
                sendFlowStatus(2);
                step++;
                return loxLead * 1000000; // delay by lox lead
            case 3: // step 3
                // check arm valve current, loxMain current, fuelMain continuity
                Valves::openFuelMainValve();
                sendFlowStatus(3);
                step++;
                return burnTime * 1000000; // delay by burn time
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
                return 0;
        }
    }
};
