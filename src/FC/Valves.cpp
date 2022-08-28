#include "Valves.h"
#include "Ducers.h"

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
                      .ocThreshold = 10.0,
                      .period = 50 * 1000,
                      .muxChannel = &HAL::muxChan7};
    
    Valve fuelGemValve = {.valveID = 9,
                  .statePacketID = 53,
                  .statusPacketID = 29,
                  .pin = HAL::valve2Pin, 
                  .expanderPin = 255,
                  .voltage = 0.0,
                  .current = 0.0,
                  .ocThreshold = 10.0,
                  .period = 50 * 1000,
                  .muxChannel = &HAL::muxChan8};
 
    Task *_toggleFuelGemValve;
    Task *_toggleLoxGemValve;

    bool fuelGemOpen = false;
    bool loxGemOpen = false;

    bool loxGemValveAbovePressure = false;
    bool fuelGemValveAbovePressure = false;

    void sendStatusPacket() {
        Comms::Packet tmp = {.id = 49};
        Comms::packetAddUint16(&tmp, valveStates);
        Comms::emitPacket(&tmp);
    }

    // common function for opening a valve
    void openValve(Valve *valve) {
        DEBUG("Opening valve\n");
        DEBUG_FLUSH();
        digitalWriteFast(valve->pin, HIGH); // turn on the physical pin
        DEBUG("Opened valve\n");
        DEBUG_FLUSH();
        valveStates |= (0x01 << valve->valveID); // set bit <valveID> to 1

        Comms::Packet tmp = {.id = valve->statePacketID}; // valve packets have an offset of 40 (check the E-1 Design spreadsheet)
        Comms::packetAddUint8(&tmp, 1); // a value of 1 indicates the valve was turned on
        Comms::emitPacket(&tmp);
        
        sendStatusPacket();
    }

    // common function for closing a valve
    void closeValve(Valve *valve, uint8_t OCShutoff) { //optional argument overcurrentShutoff
        DEBUG("Closing valve\n");
        DEBUG_FLUSH();
        digitalWriteFast(valve->pin, LOW); // turn off the physical pin
        DEBUG("Closed valve\n");
        DEBUG_FLUSH();
        valveStates &= ~(0x01 << valve->valveID); // set bit <valveID> to 1

        Comms::Packet tmp = {.id = valve->statePacketID}; // valve packets have an offfset of 40 (check the E-1 Design spreadsheet)
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
        valve->current = valve->muxChannel->readChannel1();
        valve->voltage = valve->muxChannel->readChannel2();
        DEBUG("Valve current: ");
        DEBUG(valve->current);
        DEBUG("\n");
        DEBUG_FLUSH();
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
            DEBUG("Fuel gems toggled TRUE, closing\n");
            DEBUG_FLUSH();
            _toggleFuelGemValve->enabled = true;
        } else {
            DEBUG("Fuel gems toggled FALSE, closing\n");
            DEBUG_FLUSH();
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
            DEBUG("Closing fuel gems - toggling \n");
            DEBUG_FLUSH();
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

    Comms::Packet autoventPacket = {.id = 51};

    uint32_t autoventFuelGemValveTask() {
        float fuelPressure = Ducers::fuelTankPTValue;

        if (fuelPressure > autoVentUpperThreshold) {
            Valves::openFuelGemValve();
            fuelGemValveAbovePressure = true;

            autoventPacket.len = 1;
            autoventPacket.data[0] = 1;
            Comms::emitPacket(&autoventPacket);
        } else if (fuelPressure < autoVentLowerThreshold && fuelGemValveAbovePressure) {
            DEBUG("Closing fuel gems for autovent\n");
            DEBUG_FLUSH();
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
