#include "Automation.h"

namespace Automation {
    Task *flowTask = nullptr;
    Task *abortFlowTask = nullptr;

    uint32_t loxLead = Util::millisToMicros(190);
    uint32_t burnTime = Util::secondsToMicros(30);
    uint32_t ventTime = Util::millisToMicros(200);

    bool automationEnabled = false;

    bool igniterEnabled = true;
    bool breakwireEnabled = true;

    bool igniterTriggered = false;
    bool breakwireBroken = false;

    bool loxGemValveAbovePressure = false;
    bool fuelGemValveAbovePressure = false;

    Comms::Packet openLoxGemsPacket = {.id = 126};
    Comms::Packet openFuelGemsPacket = {.id = 127};

    void initAutomation(Task *flowTask, Task *abortFlowTask) {
        Automation::flowTask = flowTask;
        Automation::abortFlowTask = abortFlowTask;

        Comms::registerCallback(150, beginFlow);
        Comms::registerCallback(151, beginManualAbortFlow);
        Comms::registerCallback(152, handleAutoSettings);
        Comms::registerCallback(42, handleEnableLaunch);
    }

    void handleEnableLaunch(Comms::Packet recv, uint8_t ip) {
        if(recv.data[0] > 0) {
            automationEnabled = true;
        } else {
            automationEnabled = false;
        }
        Comms::Packet tmp = {.id = 42};
        Comms::packetAddUint8(&tmp, automationEnabled ? 1 : 0);
        Comms::emitPacket(&tmp);
    }

    void handleAutoSettings(Comms::Packet recv, uint8_t ip) {
        if(recv.len > 0) {
            // set relavent settings
            loxLead = Comms::packetGetUint32(&recv, 0);
            burnTime = Comms::packetGetUint32(&recv, 4);
            igniterEnabled = Comms::packetGetUint8(&recv, 8);
            breakwireEnabled = Comms::packetGetUint8(&recv, 9);
        }
        Comms::Packet tmp = {.id = recv.id};
        Comms::packetAddUint32(&tmp, loxLead);
        Comms::packetAddUint32(&tmp, burnTime);
        Comms::packetAddUint8(&tmp, igniterEnabled);
        Comms::packetAddUint8(&tmp, breakwireEnabled);
        Comms::emitPacket(&tmp);
    }

    Comms::Packet flowPacket = {.id = 80};
    int step = 0;

    void beginFlow(Comms::Packet packet, uint8_t ip) {
        if(!flowTask->enabled && automationEnabled) {
            step = 0;
            //reset values
            igniterTriggered = false;
            breakwireBroken = false;
            flowTask->nexttime = micros();
            flowTask->enabled = true;
        }
    }
    inline void sendFlowStatus(uint8_t status) {
        // DEBUG("FLOW STATUS: ");
        // DEBUG(status);
        // DEBUG("\n");
        flowPacket.len = 1;
        flowPacket.data[0] = status;
        Comms::emitPacket(&flowPacket);
    }

    uint32_t flow() {
        // DEBUG("STEP: ");
        // DEBUG(step);
        // DEBUG("\n");
        switch(step) {
            case 0: // step 0 (enable and actuate igniter)
                //TODO: don't even turn on igniter if igniterEnabled = False
                if(Valves::breakWire.voltage > breakWireThreshold || !breakwireEnabled) {
                    Valves::enableIgniter();
                    Valves::activateIgniter();
                    sendFlowStatus(STATE_ACTIVATE_IGNITER);
                    step++;
                    return 500 * 1000; // delay 0.5s
                } else {
                    sendFlowStatus(STATE_ABORT_BREAKWIRE_BROKEN);
                    beginAbortFlow();
                    return 0;
                }
            case 1: // step 1 after short period turn off igniter and disable igniter power
                Valves::disableIgniter();
                Valves::deactivateIgniter();
                sendFlowStatus(STATE_DEACTIVATE_DISABLE_IGNITER);
                step++;
                return 1500 * 1000; // delay 1.5s

            case 2: // step 2
                // check igniter current trigger and break wire
                if ((igniterTriggered || !igniterEnabled)
                        && (breakwireBroken || !breakwireEnabled)) {
                    Valves::openArmValve();
                    sendFlowStatus(STATE_OPEN_ARM_VALVE);
                    step++;
                    return 500 * 1000; // delay 0.5s
                } else {
                    sendFlowStatus(STATE_ABORT_BREAKWIRE_UNBROKEN);
                    beginAbortFlow();
                    return 0;
                }

            case 3: // step 3
                // check arm valve current, main valve continuity else abort
                if (Valves::armValve.current > currentThreshold) {
                    Valves::openLoxMainValve();
                    sendFlowStatus(STATE_OPEN_LOX_VALVE);
                    step++;
                    return loxLead; // delay by lox lead
                } else {
                    sendFlowStatus(STATE_ABORT_ARM_VALVE_LOW_CURRENT);
                    beginAbortFlow();
                    return 0;
                }

            case 4: // step 4
                // check arm valve current, loxMain current, fuelMain continuity
                if (Valves::armValve.current > currentThreshold
                        && Valves::loxMainValve.current > mainValveCurrentThreshold) {
                    Valves::openFuelMainValve();
                    //begin checking thermocouple values
                    sendFlowStatus(STATE_OPEN_FUEL_VALVE);
                    step++;
                    return ventTime;
                } else {
                    sendFlowStatus(STATE_ABORT_ARM_OR_LOX_VALVE_LOW_CURRENT);
                    beginAbortFlow();
                    return 0;
                }

            case 5:
                if (Valves::fuelMainValve.current > mainValveCurrentThreshold) {
                    Valves::closeArmValve(); //close arm to allow vent
                    Valves::activateMainValveVent(); //vent 1

                    step++;
                    return 2 * 1000 * 1000 - ventTime;
                } else {
                    sendFlowStatus(STATE_ABORT_FUEL_VALVE_LOW_CURRENT);
                    beginAbortFlow();
                    return 0;
                }

            case 6: // enable Load Cell abort
                // checkForLCAbortTask->enabled = true;
                //begin checking loadcell values
                sendFlowStatus(STATE_BEGIN_THRUST_CHECK);
                step++;
                return burnTime - (50 * 1000) - (2 * 1000 * 1000) - ventTime; //delay by burn time - 2 seconds - 200 ms

            case 7:
                Valves::closeLoxMainValve();
                Valves::closeFuelMainValve();
                Valves::deactivateMainValveVent();

                step++;
                return 50 * 1000;

            case 8: // step 8 (arm main to close valves)
                Valves::openArmValve(); // reopen arm to close valves
                sendFlowStatus(STATE_CLOSE_FUEL_VALVE);
                sendFlowStatus(STATE_CLOSE_LOX_VALVE);
                step++;
                return 500 * 1000;

            case 9: // step 9 (close arm valve)
                Valves::closeArmValve(); 
                sendFlowStatus(STATE_CLOSE_ARM_VALVE);
                step++;
                return 1500; //delay for gap in status messages

            default: // end
                flowTask->enabled = false;
                sendFlowStatus(STATE_NOMINAL_END_FLOW);
                return 0;
        }
    }

    void beginManualAbortFlow(Comms::Packet packet, uint8_t ip) {
        // beginAbortFlow();
        flowTask->enabled = false;
        Valves::deactivateIgniter();
        // Send packet to FC to open Lox Gems
        Comms::packetAddUint8(&openLoxGemsPacket, 1);
        Comms::emitPacket(&openLoxGemsPacket, 42);
        // Send packet to FC to open Fuel Gems
        Comms::packetAddUint8(&openFuelGemsPacket, 1);
        Comms::emitPacket(&openFuelGemsPacket, 42);
        sendFlowStatus(STATE_MANUAL_SAFE_ABORT);
    }

    void beginAbortFlow() {
        if(!abortFlowTask->enabled) {
            step = 0;
            flowTask->enabled = false;
            abortFlowTask->nexttime = micros() + 1500; // 1500 is a dirty hack to make sure flow status gets recorded. Ask @andy
            abortFlowTask->enabled = true;
        }
    }

    uint32_t abortFlow() {
        // DEBUG("ABORT STEP: ");
        // DEBUG(step);
        // DEBUG("\n");
        switch(step) {
            case 0: // deactivate igniter and vent pneumatics and tanks
                 // Send packet to FC to open Lox Gems
                Comms::packetAddUint8(&openLoxGemsPacket, 1);
                Comms::emitPacket(&openLoxGemsPacket, 42);
                // Send packet to FC to open Fuel Gems
                Comms::packetAddUint8(&openFuelGemsPacket, 1);
                Comms::emitPacket(&openFuelGemsPacket, 42);

                Valves::deactivateIgniter();

                Valves::closeArmValve(); //close arm to allow vent
                Valves::activateMainValveVent(); //vent 1

                step++;
                return ventTime;
            case 1: // close 5ways
                Valves::closeLoxMainValve();
                Valves::closeFuelMainValve();
                Valves::deactivateMainValveVent();

                step++;
                return 50 * 1000; //delay to allow solenoid actuation before re-arm
            case 2: // physically close valves
                Valves::openArmValve(); // reopen arm to close valves

                step++;
                return 500 * 1000;
            default: // end
                Valves::closeArmValve(); 
                abortFlowTask->enabled = false;
                sendFlowStatus(STATE_ABORT_END_FLOW);
                return 0;
        }
    }

    uint32_t checkIgniter() {
        igniterTriggered = Valves::igniter.current > igniterTriggerThreshold || igniterTriggered;
        breakwireBroken = Valves::breakWire.voltage < breakWireThreshold || breakwireBroken;
        return Valves::igniter.period; //TODO determine appropriate sampling time
    }

};
