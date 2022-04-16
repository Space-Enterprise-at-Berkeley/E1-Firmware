#include "Automation.h"

namespace Automation {
    Task *flowTask = nullptr;
    Task *abortFlowTask = nullptr;
    Task *checkForTCAbortTask = nullptr;
    Task *checkForLCAbortTask = nullptr;

    uint32_t loxLead = Util::millisToMicros(165);
    uint32_t burnTime = Util::secondsToMicros(25);
    uint32_t ventTime = Util::millisToMicros(200);

    bool igniterEnabled = true;
    bool breakwireEnabled = true;
    bool thrustEnabled = true;

    bool igniterTriggered = false;

    bool loxGemValveAbovePressure = false;
    bool fuelGemValveAbovePressure = false;

    float loadCell12Value;
    float loadCell34Value;
    uint32_t lastLoadCell12Time; // last time that load cell value was received
    uint32_t lastLoadCell34Time; // last time that load cell value was received

    //each index maps to a check
    uint8_t hysteresisValues[6] = {0};
    uint8_t hysteresisThreshold = 10;

    void initAutomation(Task *flowTask, Task *abortFlowTask, Task *checkForTCAbortTask, Task *checkForLCAbortTask) {
        Automation::flowTask = flowTask;
        Automation::abortFlowTask = abortFlowTask;
        Automation::checkForTCAbortTask = checkForTCAbortTask;
        Automation::checkForLCAbortTask = checkForLCAbortTask;

        Comms::registerCallback(150, beginFlow);
        Comms::registerCallback(151, beginManualAbortFlow);
        Comms::registerCallback(120, readLoadCell);
        Comms::registerCallback(152, handleAutoSettings);
    }

    void handleAutoSettings(Comms::Packet recv, uint8_t ip) {
        if(recv.len > 0) {
            // set relavent settings
            loxLead = Comms::packetGetUint32(&recv, 0);
            burnTime = Comms::packetGetUint32(&recv, 4);
            igniterEnabled = Comms::packetGetUint8(&recv, 8);
            breakwireEnabled = Comms::packetGetUint8(&recv, 9);
            thrustEnabled = Comms::packetGetUint8(&recv, 10);
        }
        Comms::Packet tmp = {.id = recv.id};
        Comms::packetAddUint32(&tmp, loxLead);
        Comms::packetAddUint32(&tmp, burnTime);
        Comms::packetAddUint8(&tmp, igniterEnabled);
        Comms::packetAddUint8(&tmp, breakwireEnabled);
        Comms::packetAddUint8(&tmp, thrustEnabled);
        Comms::emitPacket(&tmp);
    }

    Comms::Packet flowPacket = {.id = 50};
    int step = 0;

    void beginFlow(Comms::Packet packet, uint8_t ip) {
        if(!flowTask->enabled) {
            step = 0;
            Ducers::ptUpdatePeriod = 1 * 1000;
            //reset values
            igniterTriggered = false;
            flowTask->nexttime = micros();
            flowTask->enabled = true;
            checkForTCAbortTask->enabled = false;
            checkForLCAbortTask->enabled = false;
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
                        && (Valves::breakWire.voltage < breakWireThreshold || !breakwireEnabled)) {
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
                    checkForTCAbortTask->enabled = true;
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
                checkForLCAbortTask->enabled = true;
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
                
                checkForTCAbortTask->enabled = false;
                checkForLCAbortTask->enabled = false;
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
        Valves::deactivateIgniter();
        Valves::openLoxGemValve();
        Valves::openFuelGemValve();
        sendFlowStatus(STATE_MANUAL_SAFE_ABORT);
    }

    void beginAbortFlow() {
        if(!abortFlowTask->enabled) {
            step = 0;
            flowTask->enabled = false;
            abortFlowTask->nexttime = micros() + 1500; // 1500 is a dirty hack to make sure flow status gets recorded. Ask @andy
            abortFlowTask->enabled = true;
            checkForTCAbortTask->enabled = false;
            checkForLCAbortTask->enabled = false;
        }
    }

    uint32_t abortFlow() {
        // DEBUG("ABORT STEP: ");
        // DEBUG(step);
        // DEBUG("\n");
        switch(step) {
            case 0: // deactivate igniter and vent pneumatics and tanks
                Valves::openLoxGemValve();
                Valves::openFuelGemValve();

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
        return Valves::igniter.period; //TODO determine appropriate sampling time
    }

    void readLoadCell(Comms::Packet packet, uint8_t ip) {
        float loadCellSum = Comms::packetGetFloat(&packet, 8);
        if(ip == 11) {
            loadCell12Value = loadCellSum;
            lastLoadCell12Time = millis();
        } else if(ip == 12) {
            loadCell34Value = loadCellSum;
            lastLoadCell34Time = millis();
        }
        // DEBUG(loadCell12Value);
        // DEBUG("\n");
    }

    uint32_t checkForTCAbort() {
        //check thermocouple temperatures to be below a threshold
        float maxThermocoupleValue = max(max(Thermocouples::engineTC0Value, Thermocouples::engineTC1Value), 
                                        max(Thermocouples::engineTC2Value, Thermocouples::engineTC3Value));

        // DEBUG("LOAD CELL VALUE: ");
        // DEBUG(loadCell12Value);
        // DEBUG(" TC VALUE: ");
        // DEBUG(Thermocouples::engineTC3Value);
        // DEBUG(" TC ROC: ");
        // DEBUG(Thermocouples::engineTC3ROC);
        // DEBUG(" Hysteresis TC3: ");
        // DEBUG(hysteresisValues[4]);
        // DEBUG("\n");

        if (maxThermocoupleValue > thermocoupleAbsoluteThreshold) {
            hysteresisValues[0] += 1;
            if (hysteresisValues[0] >= hysteresisThreshold) {
                sendFlowStatus(STATE_ABORT_ENGINE_TEMP);
                beginAbortFlow();
            }
        } else {
            hysteresisValues[0] = 0;
        }

        if (Thermocouples::engineTC0Value > thermocoupleThreshold && Thermocouples::engineTC0ROC > thermocoupleRateThreshold) {
            hysteresisValues[1] += 1;
            if (hysteresisValues[1] >= hysteresisThreshold) {
                sendFlowStatus(STATE_ABORT_ENGINE_TEMP_ROC);
                beginAbortFlow();
            }
        } else {
            hysteresisValues[1] = 0;
        }

        if (Thermocouples::engineTC1Value > thermocoupleThreshold && Thermocouples::engineTC1ROC > thermocoupleRateThreshold) {
            hysteresisValues[2] += 1;
            if (hysteresisValues[2] >= hysteresisThreshold) {
                sendFlowStatus(STATE_ABORT_ENGINE_TEMP_ROC);
                beginAbortFlow();
            }
        } else {
            hysteresisValues[2] = 0;
        }

        if (Thermocouples::engineTC2Value > thermocoupleThreshold && Thermocouples::engineTC2ROC > thermocoupleRateThreshold) {
            hysteresisValues[3] += 1;
            if (hysteresisValues[3] >= hysteresisThreshold) {
                sendFlowStatus(STATE_ABORT_ENGINE_TEMP_ROC);
                beginAbortFlow();
            }
        } else {
            hysteresisValues[3] = 0;
        }

        if (Thermocouples::engineTC3Value > thermocoupleThreshold && Thermocouples::engineTC3ROC > thermocoupleRateThreshold) {
            hysteresisValues[4] += 1;
            if (hysteresisValues[4] >= hysteresisThreshold) {
                sendFlowStatus(STATE_ABORT_ENGINE_TEMP_ROC);
                beginAbortFlow();
            }
        } else {
            hysteresisValues[4] = 0;
        }

        return Thermocouples::tcUpdatePeriod; //TODO determine appropriate sampling time
    }

    uint32_t checkForLCAbort() {
        DEBUG(loadCell12Value+loadCell34Value);
        DEBUG(" : ");
        DEBUG(millis() - lastLoadCell12Time);
        DEBUG(" : ");
        DEBUG(millis() - lastLoadCell34Time);
        DEBUG(" : ");
        DEBUG(thrustEnabled);
        DEBUG("\n");
        if (loadCell12Value+loadCell34Value < loadCellThreshold && millis() - lastLoadCell12Time < 25 && millis() - lastLoadCell34Time < 25 && thrustEnabled) {
            hysteresisValues[5] += 1;
            if (hysteresisValues[5] >= hysteresisThreshold) {
                sendFlowStatus(STATE_ABORT_THRUST);
                beginAbortFlow();
            }
        } else {
            hysteresisValues[5] = 0;
        }

        return 12500; // load cells are sampled at 80 hz
    }

    Comms::Packet autoventPacket = {.id = 51};
    uint32_t autoventFuelGemValveTask() {
        float fuelPresure = Ducers::fuelTankPTValue;

        if (fuelPresure > autoVentUpperThreshold) {
            Valves::openFuelGemValve();
            fuelGemValveAbovePressure = true;

            autoventPacket.len = 1;
            autoventPacket.data[0] = 1;
            Comms::emitPacket(&autoventPacket);
        } else if (fuelPresure < autoVentLowerThreshold && fuelGemValveAbovePressure) {
            Valves::closeFuelGemValve();
            fuelGemValveAbovePressure = false;
        }

        return 0.25 * 1e6;
    }

    uint32_t autoventLoxGemValveTask() {
        float loxPressure = Ducers::loxTankPTValue;

        if (loxPressure > autoVentUpperThreshold) {
            Valves::openLoxGemValve();
            loxGemValveAbovePressure = true;

            autoventPacket.len = 1;
            autoventPacket.data[0] = 0;
            Comms::emitPacket(&autoventPacket);
        } else if (loxPressure < autoVentLowerThreshold && loxGemValveAbovePressure) {
            Valves::closeLoxGemValve();
            loxGemValveAbovePressure = false;
        }

        return 0.25 * 1e6;
    }
};
