#include "Valves.h"

namespace Valves {
    // state of each valve is stored in each bit
    // bit 0 is armValve, bit 1 is igniter, etc
    // the order of bits is the same as the order of valves on the E-1 Design spreadsheet
    uint16_t valveStates = 0x00;

    // info for each individual valve
    Valve armValve = {.valveID = 0,
                      .statePacketID = 40,
                      .statusPacketID = 30,
                      .pin = 255,
                      .expanderPin = HAL::chan11Pin,
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 2.0,
                      .period = 50 * 1000,
                      .ina = &HAL::chan11};

    Valve igniter = {.valveID = 1,
                      .statePacketID = 41,
                      .statusPacketID = 31,
                      .pin = 255,
                      .expanderPin = HAL::chan7Pin,
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 2.0,
                      .period = 50 * 1000,
                      .ina = &HAL::chan7};
    
    Valve loxMainValve = {.valveID = 2,
                      .statePacketID = 42,
                      .statusPacketID = 32,
                      .pin = 255,
                      .expanderPin = HAL::chan8Pin,
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 2.0,
                      .period = 50 * 1000,
                      .ina = &HAL::chan8};

    Valve fuelMainValve = {.valveID = 3,
                      .statePacketID = 43,
                      .statusPacketID = 33,
                      .pin = 255,
                      .expanderPin = HAL::chan10Pin,
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 2.0,
                      .period = 50 * 1000,
                      .ina = &HAL::chan10};

    Valve breakWire = {.valveID = 255, // break wire can't be actuated, no valveID is used.
                      .statePacketID = 0,
                      .statusPacketID = 34,
                      .pin = HAL::chan3Pin,
                      .expanderPin = 255,
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 0.1,
                      .period = 50 * 1000,
                      .ina = &HAL::chan3};
    
    Valve RQD = {.valveID = 5, // actuated from the IO Expander
                .statePacketID = 45,
                .statusPacketID = 35,
                .pin = HAL::chan4Pin, // dont use pin
                .expanderPin = 255,
                .voltage = 0.0,
                .current = 0.0,
                .ocThreshold = 3.0,
                .period = 50 * 1000,
                .ina = &HAL::chan4};

    Valve mainValveVent = {.valveID = 6, // actuated from the IO Expander
                      .statePacketID = 46,
                      .statusPacketID = 36,
                      .pin = HAL::chan5Pin, // dont use pin
                      .expanderPin = 255,
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 3.0,
                      .period = 50 * 1000,
                      .ina = &HAL::chan5};

    Valve igniterEnableRelay = {.valveID = 4, // actuated from the IO Expander
                      .statePacketID = 48,
                      .statusPacketID = 38,
                      .pin = HAL::chan2Pin,
                      .expanderPin = 255,
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 3.0,
                      .period = 100 * 1000,
                      .ina = &HAL::chan2};

    //TODO fill in with actual GEM pin values
    Valve loxGemValve = {.valveID = 8,
                      .statePacketID = 52,
                      .statusPacketID = 28,
                      .pin = 255, // dont use pin
                      .expanderPin = HAL::chan9Pin,
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 3.0,
                      .period = 50 * 1000,
                      .ina = &HAL::chan9};

    Valve fuelGemValve = {.valveID = 9,
                      .statePacketID = 53,
                      .statusPacketID = 29,
                      .pin = 255, // dont use pin
                      .expanderPin = HAL::chan6Pin,
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 3.0,
                      .period = 50 * 1000,
                      .ina = &HAL::chan6};

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
        if(valve->pin != 255) {
            digitalWriteFast(valve->pin, HIGH); // turn on the physical pin
        } else {
            HAL::ioExpander.turnOn(valve->expanderPin);
        }
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
        valve->voltage = valve->ina->readBusVoltage();
        valve->current = valve->ina->readShuntCurrent();
        
        Comms::Packet tmp = {.id = valve->statusPacketID};
        //IDS: Arming valve, igniter, lox main valve, fuel main valve
        if (valve->current > valve->ocThreshold) {
            closeValve(valve, 1);
        }
        Comms::packetAddFloat(&tmp, valve->voltage);
        Comms::packetAddFloat(&tmp, valve->current);
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
    void initValves(Task *toggleLoxGemValveTask, Task *toggleFuelGemValveTask) {
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

};