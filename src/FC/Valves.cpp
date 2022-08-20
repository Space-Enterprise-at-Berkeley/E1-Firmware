#include "Valves.h"

namespace Valves {
    // state of each valve is stored in each bit
    // bit 0 is armValve, bit 1 is igniter, etc
    // the order of bits is the same as the order of valves on the E-1 Design spreadsheet
    uint16_t valveStates = 0x00;

    // Info for each individual valve.
    // Mapping according to vertical system wiring + stack schematic, 08/17

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
                  .pin = HAL::valve2Pin, 
                  .expanderPin = 255,
                  .voltage = 0.0,
                  .current = 0.0,
                  .ocThreshold = 3.0,
                  .period = 50 * 1000,
                  .muxChannel = &HAL::muxChan8};

    Valve breakWire = {.valveID = 255, // break wire can't be actuated, no valveID is used.
                        .statePacketID = 0,
                        .statusPacketID = 34,
                        .pin = 255, // only reading continuity with MUX 
                        .expanderPin = 255,
                        .voltage = 0.0,
                        .current = 0.0,
                        .ocThreshold = 0.1,
                        .period = 50 * 1000,
                        .muxChannel = &HAL::muxChan4}; // Using Breakwire1, S5B on MUX (MUX channel 4)

    // todo: define these for the fcv3 channels
    Valve chute1 = {};
    
    Valve chute2 = {};

    Valve cam1 = {};

    Valve cam2 = {};

    Valve radio = {};
 
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
        digitalWriteFast(valve->pin, HIGH); // turn on the physical pin
        valveStates |= (0x01 << valve->valveID); // set bit <valveID> to 1

        Comms::Packet tmp = {.id = valve->statePacketID}; // valve packets have an offset of 40 (check the E-1 Design spreadsheet)
        Comms::packetAddUint8(&tmp, 1); // a value of 1 indicates the valve was turned on
        Comms::emitPacket(&tmp);
        
        sendStatusPacket();
    }

    // common function for closing a valve
    void closeValve(Valve *valve, uint8_t OCShutoff) { //optional argument overcurrentShutoff
        digitalWriteFast(valve->pin, LOW); // turn off the physical pin
        valveStates &= ~(0x01 << valve->valveID); // set bit <valveID> to 1

        Comms::Packet tmp = {.id = valve->statePacketID}; // valve packets have an offset of 40 (check the E-1 Design spreadsheet)
        Comms::packetAddUint8(&tmp, OCShutoff << 1); // a value of 0 indicates the valve was turned off, 2 indicates overcurrent shutoff
        Comms::emitPacket(&tmp);
        
        sendStatusPacket();
    }

    // functions for each individual valve
    void openFuelGemValve() { openValve(&fuelGemValve); }
    void closeFuelGemValve(uint8_t OCShutoff = 0) { closeValve(&fuelGemValve, OCShutoff); }
    void fuelGemValvePacketHandler(Comms::Packet tmp, uint8_t ip) { return tmp.data[0] ? openFuelGemValve() : closeFuelGemValve(); }

    void openLoxGemValve() { openValve(&loxGemValve); }
    void closeLoxGemValve(uint8_t OCShutoff = 0) { closeValve(&loxGemValve, OCShutoff); }
    void loxGemValvePacketHandler(Comms::Packet tmp, uint8_t ip) { return tmp.data[0] ? openLoxGemValve() : closeLoxGemValve(); }

    // common function for sampling a valve's voltage and current
    void sampleValve(Valve *valve) {
        valve->voltage = valve->muxChannel->readChannel1();
        valve->current = valve->muxChannel->readChannel2();
        
        Comms::Packet tmp = {.id = valve->statusPacketID};
        if (valve->current > valve->ocThreshold) {
            closeValve(valve, 1);
        }
        Comms::packetAddFloat(&tmp, valve->voltage);
        Comms::packetAddFloat(&tmp, valve->current);
        Comms::emitPacket(&tmp);
    }

    // individual task functions (see taskTable in main.cpp)
    uint32_t loxGemValveSample() {
        sampleValve(&loxGemValve);
        return fuelGemValve.period;
    }

    uint32_t fuelGemValveSample() {
        sampleValve(&fuelGemValve);
        return loxGemValve.period;
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
        Comms::registerCallback(126, loxGemValvePacketHandler);
        Comms::registerCallback(127, fuelGemValvePacketHandler);

        Comms::registerCallback(128, toggleLoxGemValve);
        Comms::registerCallback(129, toggleFuelGemValve);
       
        _toggleLoxGemValve = toggleLoxGemValveTask;
        _toggleFuelGemValve = toggleFuelGemValveTask;
    }

};
