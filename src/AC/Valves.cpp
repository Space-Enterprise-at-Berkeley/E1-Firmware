#include "Valves.h"

namespace Valves {
    // state of each valve is stored in each bit
    // bit 0 is armValve, bit 1 is igniter, etc
    // the order of bits is the same as the order of valves on the E-1 Design spreadsheet
    uint16_t valveStates = 0x00;

    // info for each individual valve
    Valve loxDomeHeater = {.valveID = 0,
                      .statePacketID = 104,
                      .statusPacketID = 102,
                      .pin = HAL::ctl24vChan1,
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 3.0,
                      .period = 10 * 1000,
                      .ina = &HAL::chan2};

    Valve fuelDomeHeater = {.valveID = 1,
                      .statePacketID = 105,
                      .statusPacketID = 103,
                      .pin = HAL::ctl24vChan2,
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 3.0,
                      .period = 10 * 1000,
                      .ina = &HAL::chan3};

    void sendStatusPacket() {
        Comms::Packet tmp = {.id = 107};
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
    void openLoxDomeHeater() { openValve(&loxDomeHeater); }
    void closeLoxDomeHeater(uint8_t OCShutoff = 0) { closeValve(&loxDomeHeater, OCShutoff); }
    void loxDomeHeaterPacketHandler(Comms::Packet tmp, uint8_t ip) { return tmp.data[0] ? openLoxDomeHeater() : closeLoxDomeHeater(); }

    void openFuelDomeHeater() { openValve(&fuelDomeHeater); }
    void closeFuelDomeHeater(uint8_t OCShutoff = 0) { closeValve(&fuelDomeHeater, OCShutoff); }
    void fuelDomeHeaterPacketHandler(Comms::Packet tmp, uint8_t ip) { return tmp.data[0] ? openFuelDomeHeater() : closeFuelDomeHeater(); }

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

    uint32_t loxDomeHeaterSample() {
        sampleValve(&loxDomeHeater);
        return loxDomeHeater.period;
    }

    uint32_t fuelDomeHeaterSample() {
        sampleValve(&fuelDomeHeater);
        return fuelDomeHeater.period;
    }

    // init function for valves namespace
    void initValves() {
        // link the right packet IDs to the valve open/close handler functions
        Comms::registerCallback(182, loxDomeHeaterPacketHandler);
        Comms::registerCallback(183, fuelDomeHeaterPacketHandler);
    }

};