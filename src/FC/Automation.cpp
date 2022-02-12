#include "Automation.h"
#include "Ducers.h"

namespace Automation {
    Task *flowTask = nullptr;
    Task *abortFlowTask = nullptr;
    Task *checkForTCAbortTask = nullptr;
    Task *checkForLCAbortTask = nullptr;

    uint32_t loxLead = 165 * 1000;
    uint32_t burnTime = 25 * 1000 * 1000;

    bool igniterEnabled = true;
    bool breakwireEnabled = true;
    bool thrustEnabled = true;

    bool igniterTriggered = false;

    float loadCellValue;
    uint32_t lastLoadCellTime; // last time that load cell value was received

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

    void handleAutoSettings(Comms::Packet recv) {
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

    void beginFlow(Comms::Packet packet) {
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
                        && Valves::loxMainValve.current > currentThreshold) {
                    Valves::openFuelMainValve();
                    //begin checking thermocouple values
                    checkForTCAbortTask->enabled = true;
                    sendFlowStatus(STATE_OPEN_FUEL_VALVE);
                    step++;
                    return 2 * 1000 * 1000; // delay by 2 seconds
                } else {
                    sendFlowStatus(STATE_ABORT_ARM_OR_LOX_VALVE_LOW_CURRENT);
                    beginAbortFlow();
                    return 0;
                }

            case 5: // enable Load Cell abort
                checkForLCAbortTask->enabled = true;
                //begin checking loadcell values
                sendFlowStatus(STATE_BEGIN_THRUST_CHECK);
                step++;
                return burnTime - (2 * 1000 * 1000); //delay by burn time - 2 seconds

            case 6: // step 6 (close fuel)
                Valves::closeFuelMainValve();
                checkForTCAbortTask->enabled = false;
                checkForLCAbortTask->enabled = false;
                sendFlowStatus(STATE_CLOSE_FUEL_VALVE);
                step++;
                return 200 * 1000;

            case 7: // step 7 (close lox)
                Valves::closeLoxMainValve();
                sendFlowStatus(STATE_CLOSE_LOX_VALVE);
                step++;
                return 500 * 1000;

            case 8: // step 8 (close arm valve)
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

    void beginManualAbortFlow(Comms::Packet packet) {
        // beginAbortFlow();
        Valves::deactivateIgniter();
        sendFlowStatus(STATE_MANUAL_SAFE_ABORT);
    }

    void beginAbortFlow() {
        if(!abortFlowTask->enabled) {
            flowTask->enabled = false;
            abortFlowTask->nexttime = micros() + 1500; // 1500 is a dirty hack to make sure flow status gets recorded. Ask @andy
            abortFlowTask->enabled = true;
            checkForTCAbortTask->enabled = false;
            checkForLCAbortTask->enabled = false;
        }
    }

    uint32_t abortFlow() {
        Valves::closeFuelMainValve();
        Valves::closeLoxMainValve();
        Valves::closeArmValve();
        Valves::deactivateIgniter();
        abortFlowTask->enabled = false;
        sendFlowStatus(STATE_ABORT_END_FLOW);
        return 0;
    }

    uint32_t checkIgniter() {
        igniterTriggered = Valves::igniter.current > igniterTriggerThreshold || igniterTriggered;
        return Valves::igniter.period; //TODO determine appropriate sampling time
    }

    void readLoadCell(Comms::Packet packet) {
        float loadCell1Value = Comms::packetGetFloat(&packet, 0);
        float loadCell2Value = Comms::packetGetFloat(&packet, 4);
        float loadCellSum = Comms::packetGetFloat(&packet, 8);

        loadCellValue = loadCellSum;
        lastLoadCellTime = millis();
        DEBUG(loadCellValue);
        DEBUG("\n");
    }

    uint32_t checkForTCAbort() {
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
        if (loadCellValue < loadCellThreshold && millis() - lastLoadCellTime < 25 && thrustEnabled) {
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
};
