#include "Actuators.h"

namespace Actuators {
    // state of each valve is stored in each bit
    // bit 0 is armValve, bit 1 is igniter, etc
    // the order of bits is the same as the order of valves on the E-1 Design spreadsheet
    uint16_t valveStates = 0x00;

    // Info for each individual valve.
    // Mapping according to vertical system wiring + stack schematic, 08/13
    Valve armValve = {.valveID = 0,
                      .statePacketID = 40,
                      .statusPacketID = 30,
                     // .pin = 255,
                      .pin = HAL::valve5Pin,
                      .expanderPin = 255, 
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 2.0,
                      .period = 50 * 1000,
                      .muxChannel = &HAL::muxChan11};

    Valve igniter = {.valveID = 1,
                      .statePacketID = 41,
                      .statusPacketID = 31,
                      .pin = HAL::valve6Pin,
                      .expanderPin = 255,
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 2.0,
                      .period = 50 * 1000,
                      .muxChannel = &HAL::muxChan12};
    
    Valve loxMainValve = {.valveID = 2,
                      .statePacketID = 42,
                      .statusPacketID = 32,
                      .pin = HAL::valve2Pin,
                      .expanderPin = 255,
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 2.0,
                      .period = 50 * 1000,
                      .muxChannel = &HAL::muxChan8};

    Valve fuelMainValve = {.valveID = 3,
                      .statePacketID = 43,
                      .statusPacketID = 33,
                      .pin = HAL::valve4Pin,
                      .expanderPin = 255,
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 2.0,
                      .period = 50 * 1000,
                      .muxChannel = &HAL::muxChan10};

    Valve breakWire = {.valveID = 255, // break wire can't be actuated, no valveID is used.
                      .statePacketID = 0,
                      .statusPacketID = 34,
                      .pin = 255, // HAL::chan3Pin, TODO: figure out - only reading continuity with MUX 
                      .expanderPin = 255,
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 0.1,
                      .period = 50 * 1000,
                      .muxChannel = &HAL::muxChan4}; // Using Breakwire1, S5B on MUX (MUX channel 4)
    
    Valve RQD = {.valveID = 5, // actuated from the IO Expander
                .statePacketID = 45,
                .statusPacketID = 35,
                .pin = 255, // HAL::chan4Pin, TODO: Assign...
                .expanderPin = 255,
                .voltage = 0.0,
                .current = 0.0,
                .ocThreshold = 3.0,
                .period = 50 * 1000,
                .muxChannel = &HAL::muxChan6}; // TODO: Reassign

    Valve mainValveVent = {.valveID = 6, // actuated from the IO Expander
                      .statePacketID = 46,
                      .statusPacketID = 36,
                      .pin = 255, // TODO: Assign...
                      .expanderPin = 255,
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 3.0,
                      .period = 50 * 1000,
                      .muxChannel = &HAL::muxChan7};

    Valve igniterEnableRelay = {.valveID = 4, // actuated from the IO Expander
                      .statePacketID = 48,
                      .statusPacketID = 38,
                      .pin = 255, // TODO: Assign...
                      .expanderPin = 255,
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 3.0,
                      .period = 50 * 1000,
                      .muxChannel = &HAL::muxChan8};

    //TODO fill in with actual GEM pin values
    Valve loxGemValve = {.valveID = 8,
                      .statePacketID = 52,
                      .statusPacketID = 28,
                      .pin = HAL::valve1Pin, 
                      .expanderPin = 255,
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 3.0,
                      .period = 50 * 1000,
                      .muxChannel = &HAL::muxChan7};
    
    Valve fuelGemValve = {.valveID = 9,
                  .statePacketID = 53,
                  .statusPacketID = 29,
                  .pin = HAL::hBridge3Pin1, 
                  .expanderPin = 255,
                  .voltage = 0.0,
                  .current = 0.0,
                  .ocThreshold = 3.0,
                  .period = 50 * 1000,
                  .muxChannel = &HAL::muxChan15};

    Actuator chute1 = {.statusPacketID = 0,
                    .hasVoltage = true,
                    .hasCurrent = true,
                    .voltage = 0.0,
                    .current = 0.0,
                    .period = 100 * 1000,
                    .muxChannel = &HAL::muxChan0};

    Actuator chute2 = {.statusPacketID = 0,
                    .hasVoltage = true,
                    .hasCurrent = true,
                    .voltage = 0.0,
                    .current = 0.0,
                    .period = 100 * 1000,
                    .muxChannel = &HAL::muxChan1};

    Actuator cam1 = {.statusPacketID = 0,
                    .hasVoltage = false,
                    .hasCurrent = true,
                    .voltage = 0.0,
                    .current = 0.0,
                    .period = 100 * 1000,
                    .muxChannel = &HAL::muxChan2};

    Actuator rfAmp = {.statusPacketID = 0,
                    .hasVoltage = false,
                    .hasCurrent = true,
                    .voltage = 0.0,
                    .current = 0.0,
                    .period = 100 * 1000,
                    .muxChannel = &HAL::muxChan3};

    Actuator cam2 = {.statusPacketID = 0,
                    .hasVoltage = false,
                    .hasCurrent = true,
                    .voltage = 0.0,
                    .current = 0.0,
                    .period = 100 * 1000,
                    .muxChannel = &HAL::muxChan4};

    Actuator radio = {.statusPacketID = 0,
                    .hasVoltage = false,
                    .hasCurrent = true,
                    .voltage = 0.0,
                    .current = 0.0,
                    .period = 100 * 1000,
                    .muxChannel = &HAL::muxChan5};

    Actuator hBridge1 = {.statusPacketID = 0,
                    .hasVoltage = true,
                    .hasCurrent = true,
                    .voltage = 0.0,
                    .current = 0.0,
                    .period = 100 * 1000,
                    .muxChannel = &HAL::muxChan13};

    Actuator hBridge2 = {.statusPacketID = 0,
                    .hasVoltage = true,
                    .hasCurrent = true,
                    .voltage = 0.0,
                    .current = 0.0,
                    .period = 100 * 1000,
                    .muxChannel = &HAL::muxChan14};
    // Currently h bridge 3 is being used for PROP GEMS... 
    Actuator hBridge3 = {.statusPacketID = 0,
                    .hasVoltage = true,
                    .hasCurrent = true,
                    .voltage = 0.0,
                    .current = 0.0,
                    .period = 100 * 1000,
                    .muxChannel = &HAL::muxChan9}; // TODO: Eventually need to fix - currently mapped to Valve channel 3 to avoid conflict

    Actuator break2 = {.statusPacketID = 0,
                    .hasVoltage = true,
                    .hasCurrent = false,
                    .voltage = 0.0,
                    .current = 0.0,
                    .period = 100 * 1000,
                    .muxChannel = &HAL::muxChan6}; // Ch5 in use for radio power

    Task *_toggleFuelGemValve;
    Task *_toggleLoxGemValve;

    bool fuelGemOpen = false;
    bool loxGemOpen = false;

    void sendStatusPacket() {
        Comms::Packet tmp = {.id = 49};
        Comms::packetAddUint16(&tmp, valveStates);
        Comms::emitPacket(&tmp);
    }

    // common function for opening a valve
    void openValve(Valve *valve) {
        // if(valve->pin != 255) {
        //     digitalWriteFast(valve->pin, HIGH); // turn on the physical pin
        // } else {
        //     HAL::ioExpander.turnOn(valve->expanderPin);
        // }
        digitalWriteFast(valve->pin, HIGH); // turn on the physical pin
        valveStates |= (0x01 << valve->valveID); // set bit <valveID> to 1


        Comms::Packet tmp = {.id = valve->statePacketID}; // valve packets have an offset of 40 (check the E-1 Design spreadsheet)
        Comms::packetAddUint8(&tmp, 1); // a value of 1 indicates the valve was turned on
        Comms::emitPacket(&tmp);
        
        sendStatusPacket();
    }

    // common function for closing a valve
    void closeValve(Valve *valve, uint8_t OCShutoff) { //optional argument overcurrentShutoff
        if(valve->pin != 255) {
            digitalWriteFast(valve->pin, LOW); // turn off the physical pin
        } else {
            HAL::ioExpander.turnOff(valve->expanderPin);
        }
        valveStates &= ~(0x01 << valve->valveID); // set bit <valveID> to 1

        Comms::Packet tmp = {.id = valve->statePacketID}; // valve packets have an offset of 40 (check the E-1 Design spreadsheet)
        Comms::packetAddUint8(&tmp, OCShutoff << 1); // a value of 0 indicates the valve was turned off, 2 indicates overcurrent shutoff
        Comms::emitPacket(&tmp);
        
        sendStatusPacket();
    }

    // functions for each individual valve
    void openArmValve() { openValve(&armValve); }
    void closeArmValve(uint8_t OCShutoff = 0) { closeValve(&armValve, OCShutoff); }
    void armValvePacketHandler(Comms::Packet tmp, uint8_t ip) { return tmp.data[0] ? openArmValve() : closeArmValve(); }

    void activateIgniter() { openValve(&igniter); }
    void deactivateIgniter(uint8_t OCShutoff = 0) { closeValve(&igniter, OCShutoff); }
    void igniterPacketHandler(Comms::Packet tmp, uint8_t ip) { return tmp.data[0] ? activateIgniter() : deactivateIgniter(); }

    void openLoxMainValve() { openValve(&loxMainValve); }
    void closeLoxMainValve(uint8_t OCShutoff = 0) { closeValve(&loxMainValve, OCShutoff); }
    void loxMainValvePacketHandler(Comms::Packet tmp, uint8_t ip) { return tmp.data[0] ? openLoxMainValve() : closeLoxMainValve(); }

    void openFuelMainValve() { openValve(&fuelMainValve); }
    void closeFuelMainValve(uint8_t OCShutoff = 0) { closeValve(&fuelMainValve, OCShutoff); }
    void fuelMainValvePacketHandler(Comms::Packet tmp, uint8_t ip) { return tmp.data[0] ? openFuelMainValve() : closeFuelMainValve(); }

    void openFuelGemValve() { openValve(&fuelGemValve); }
    void closeFuelGemValve(uint8_t OCShutoff = 0) { closeValve(&fuelGemValve, OCShutoff); }
    void fuelGemValvePacketHandler(Comms::Packet tmp, uint8_t ip) { return tmp.data[0] ? openFuelGemValve() : closeFuelGemValve(); }

    void openLoxGemValve() { openValve(&loxGemValve); }
    void closeLoxGemValve(uint8_t OCShutoff = 0) { closeValve(&loxGemValve, OCShutoff); }
    void loxGemValvePacketHandler(Comms::Packet tmp, uint8_t ip) { return tmp.data[0] ? openLoxGemValve() : closeLoxGemValve(); }

    void activateRQD() { openValve(&RQD); }
    void deactivateRQD(uint8_t OCShutoff = 0) { closeValve(&RQD, OCShutoff); }
    void RQDPacketHandler(Comms::Packet tmp, uint8_t ip) { return tmp.data[0] ? activateRQD() : deactivateRQD(); }

    void activateMainValveVent() { openValve(&mainValveVent); }
    void deactivateMainValveVent(uint8_t OCShutoff = 0) { closeValve(&mainValveVent, OCShutoff); }
    void mainValveVentPacketHandler(Comms::Packet tmp, uint8_t ip) { return tmp.data[0] ? activateMainValveVent() : deactivateMainValveVent(); }

    void enableIgniter() { openValve(&igniterEnableRelay); }
    void disableIgniter(uint8_t OCShutoff = 0) { closeValve(&igniterEnableRelay, OCShutoff); }
    void igniterEnableRelayPacketHandler(Comms::Packet tmp, uint8_t ip) { return tmp.data[0] ? enableIgniter() : disableIgniter(); }

    // common function for sampling a valve's voltage and current
    void sampleValve(Valve *valve) {
        // DEBUG("STARTING VALVE SAMPLE\n");
        valve->voltage = valve->muxChannel->readChannel1();
        // DEBUG("VOLTAGE READABLE \n");
        valve->current = valve->muxChannel->readChannel2();
        // DEBUG("CURRENT READABLE \n");
        
        Comms::Packet tmp = {.id = valve->statusPacketID};
        //IDS: Arming valve, igniter, lox main valve, fuel main valve
        if (valve->current > valve->ocThreshold) {
            closeValve(valve, 1);
            // DEBUG("VALVE CLOSED\n");
        }
        Comms::packetAddFloat(&tmp, valve->voltage);
        // DEBUG("VOLTAGE SENT\n");
        Comms::packetAddFloat(&tmp, valve->current);
        // DEBUG("CURRENT SENT\n");
        Comms::emitPacket(&tmp);
        // DEBUG("PACKET SENT\n");
    }

    //common function for sampling non valves on the mux
    void sampleActuator(Actuator *actuator) {
        if (actuator->hasVoltage) {
            actuator->voltage = actuator->muxChannel->readChannel1();
        }
        if (actuator->hasCurrent) {
            actuator->current = actuator->muxChannel->readChannel2();
        }

        Comms::Packet tmp = {.id = actuator->statusPacketID};

        Comms::packetAddFloat(&tmp, actuator->voltage);
        Comms::packetAddFloat(&tmp, actuator->current);
        Comms::emitPacket(&tmp);
    }

    // individual task functions (see taskTable in main.cpp)
    uint32_t armValveSample() {
        sampleValve(&armValve);
        return armValve.period;
    }

    uint32_t igniterSample() {
        sampleValve(&igniter);
        return igniter.period;
    }

    uint32_t loxMainValveSample() {
        sampleValve(&loxMainValve);
        return loxMainValve.period;
    }

    uint32_t fuelMainValveSample() {
        sampleValve(&fuelMainValve);
        return fuelMainValve.period;
    }

    uint32_t loxGemValveSample() {
        sampleValve(&loxGemValve);
        return fuelGemValve.period;
    }

    uint32_t fuelGemValveSample() {
        sampleValve(&fuelGemValve);
        return loxGemValve.period;
    }

    uint32_t breakWireSample() {
        sampleValve(&breakWire);
        return breakWire.period;
    }

    uint32_t RQDSample() {
        sampleValve(&RQD);
        return RQD.period;
    }

    uint32_t mainValveVentSample() {
        sampleValve(&mainValveVent);
        return mainValveVent.period;
    }

    uint32_t igniterEnableRelaySample() {
        sampleValve(&igniterEnableRelay);
        return igniterEnableRelay.period;
    }

    void toggleFuelGemValve(Comms::Packet packet, uint8_t ip) {
        bool toggle = packet.data[0];

        if (toggle) {
            _toggleFuelGemValve->enabled = true;
        } else {
            _toggleFuelGemValve->enabled = false;
            closeFuelGemValve();
        }
    }

    void toggleLoxGemValve(Comms::Packet packet, uint8_t ip) {
        bool toggle = packet.data[0];

        if (toggle) {
            _toggleLoxGemValve->enabled = true;
        } else {
            _toggleLoxGemValve->enabled = false;
            closeLoxGemValve();
        }
    }

    uint32_t toggleFuelGemValveTask() {
        if (fuelGemOpen) {
            closeFuelGemValve();
            fuelGemOpen = false;
            return 5e6;
        } else {
            openFuelGemValve();
            fuelGemOpen = true;
            return 1e6; //toggle every second
        }
    }

    uint32_t toggleLoxGemValveTask() {
        if (loxGemOpen) {
            closeLoxGemValve();
            loxGemOpen = false;
            return 5e6;
        } else {
            openLoxGemValve();
            loxGemOpen = true;
            return 1e6; //toggle every second
        }
    }

    // init function for valves namespace
    void initActuators(Task *toggleLoxGemValveTask, Task *toggleFuelGemValveTask) {
        // link the right packet IDs to the valve open/close handler functions
        Comms::registerCallback(130, armValvePacketHandler);
        Comms::registerCallback(131, igniterPacketHandler);
        Comms::registerCallback(132, loxMainValvePacketHandler);
        Comms::registerCallback(133, fuelMainValvePacketHandler);
        Comms::registerCallback(126, loxGemValvePacketHandler);
        Comms::registerCallback(127, fuelGemValvePacketHandler);

        Comms::registerCallback(135, RQDPacketHandler);
        Comms::registerCallback(136, mainValveVentPacketHandler);

        Comms::registerCallback(138, igniterEnableRelayPacketHandler);

        Comms::registerCallback(128, toggleLoxGemValve);
        Comms::registerCallback(129, toggleFuelGemValve);
       
        _toggleLoxGemValve = toggleLoxGemValveTask;
        _toggleFuelGemValve = toggleFuelGemValveTask;
    }

    uint32_t chute1Sample() {
        sampleActuator(&chute1);
        return chute1.period;
    }

    uint32_t chute2Sample() {
        sampleActuator(&chute2);
        return chute2.period;
    }

    uint32_t cam1Sample() {
        sampleActuator(&cam1);
        return cam1.period;
    }

    uint32_t rfAmpSample() {
        sampleActuator(&rfAmp);
        return rfAmp.period;
    }

    uint32_t cam2Sample() {
        sampleActuator(&cam2);
        return cam2.period;
    }

    uint32_t radioSample() {
        sampleActuator(&radio);
        return radio.period;
    }

    uint32_t hBridge1Sample() {
        sampleActuator(&hBridge1);
        return hBridge1.period;
    }

    uint32_t hBridge2Sample() {
        sampleActuator(&hBridge2);
        return hBridge2.period;
    }

    uint32_t hBridge3Sample() {
        sampleActuator(&hBridge3);
        return hBridge3.period;
    }

    uint32_t break2Sample() {
        sampleActuator(&break2);
        return break2.period;
    }

};
