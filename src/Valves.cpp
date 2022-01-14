#include <Valves.h>

namespace Valves {
    // state of each valve is stored in each bit
    // bit 0 is armValve, bit 1 is igniter, etc
    // the order of bits is the same as the order of valves on the E-1 Design spreadsheet
    uint8_t valveStates = 0x00;

    // info for each individual valve
    Valve armValve = {.valveID = 0,
                      .statePacketID = 40,
                      .statusPacketID = 30,
                      .pin = HAL::chan0Pin,
                      .expanderPin = 255,
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 2.0,
                      .period = 50 * 1000,
                      .ina = &HAL::chan0};

    Valve igniter = {.valveID = 1,
                      .statePacketID = 41,
                      .statusPacketID = 31,
                      .pin = HAL::chan4Pin,
                      .expanderPin = 255,
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 2.0,
                      .period = 50 * 1000,
                      .ina = &HAL::chan4};
    
    Valve loxMainValve = {.valveID = 2,
                      .statePacketID = 42,
                      .statusPacketID = 32,
                      .pin = HAL::chan2Pin,
                      .expanderPin = 255,
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 2.0,
                      .period = 50 * 1000,
                      .ina = &HAL::chan2};

    Valve fuelMainValve = {.valveID = 3,
                      .statePacketID = 43,
                      .statusPacketID = 33,
                      .pin = HAL::chan3Pin,
                      .expanderPin = 255,
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 2.0,
                      .period = 50 * 1000,
                      .ina = &HAL::chan3};

    Valve breakWire = {.valveID = 255, // break wire can't be actuated
                      .statePacketID = 0,
                      .statusPacketID = 34,
                      .pin = HAL::chan5Pin,
                      .expanderPin = 255,
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 0.1,
                      .period = 50 * 1000,
                      .ina = &HAL::chan5};
    
    Valve loxTankBottomHtr = {.valveID = 5, // this valve is actuated from the IO Expander
                      .statePacketID = 45,
                      .statusPacketID = 35,
                      .pin = 255, // dont use pin
                      .expanderPin = HAL::chan7Pin,
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 3.0,
                      .period = 50 * 1000,
                      .ina = &HAL::chan7};

    Valve loxTankMidHtr = {.valveID = 6,
                      .statePacketID = 46,
                      .statusPacketID = 36,
                      .pin = 255, // dont use pin
                      .expanderPin = HAL::chan8Pin,
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 3.0,
                      .period = 50 * 1000,
                      .ina = &HAL::chan8};

    Valve loxTankTopHtr = {.valveID = 7,
                      .statePacketID = 47,
                      .statusPacketID = 37,
                      .pin = 255, // dont use pin
                      .expanderPin = HAL::chan9Pin,
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 3.0,
                      .period = 50 * 1000,
                      .ina = &HAL::chan9};

    void sendStatusPacket() {
        Comms::Packet tmp = {.id = 49};
        Comms::packetAddUint8(&tmp, valveStates);
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
    void armValvePacketHandler(Comms::Packet tmp) { return tmp.data[0] ? openArmValve() : closeArmValve(); }

    void activateIgniter() { openValve(&igniter); }
    void deactivateIgniter(uint8_t OCShutoff = 0) { closeValve(&igniter, OCShutoff); }
    void igniterPacketHandler(Comms::Packet tmp) { return tmp.data[0] ? activateIgniter() : deactivateIgniter(); }

    void openLoxMainValve() { openValve(&loxMainValve); }
    void closeLoxMainValve(uint8_t OCShutoff = 0) { closeValve(&loxMainValve, OCShutoff); }
    void loxMainValvePacketHandler(Comms::Packet tmp) { return tmp.data[0] ? openLoxMainValve() : closeLoxMainValve(); }

    void openFuelMainValve() { openValve(&fuelMainValve); }
    void closeFuelMainValve(uint8_t OCShutoff = 0) { closeValve(&fuelMainValve, OCShutoff); }
    void fuelMainValvePacketHandler(Comms::Packet tmp) { return tmp.data[0] ? openFuelMainValve() : closeFuelMainValve(); }

    void activateLoxTankBottomHtr() { openValve(&loxTankBottomHtr); }
    void deactivateLoxTankBottomHtr(uint8_t OCShutoff = 0) { closeValve(&loxTankBottomHtr, OCShutoff); }
    void loxTankBottomHtrPacketHandler(Comms::Packet tmp) { return tmp.data[0] ? activateLoxTankBottomHtr() : deactivateLoxTankBottomHtr(); }

    void activateLoxTankMidHtr() { openValve(&loxTankMidHtr); }
    void deactivateLoxTankMidHtr(uint8_t OCShutoff = 0) { closeValve(&loxTankMidHtr, OCShutoff); }
    void loxTankMidHtrPacketHandler(Comms::Packet tmp) { return tmp.data[0] ? activateLoxTankMidHtr() : deactivateLoxTankMidHtr(); }

    void activateLoxTankTopHtr() { openValve(&loxTankTopHtr); }
    void deactivateLoxTankTopHtr(uint8_t OCShutoff = 0) { closeValve(&loxTankTopHtr, OCShutoff); }
    void loxTankTopHtrPacketHandler(Comms::Packet tmp) { return tmp.data[0] ? activateLoxTankTopHtr() : deactivateLoxTankTopHtr(); }

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

    uint32_t loxTankBottomHtrSample() {
        sampleValve(&loxTankBottomHtr);
        return loxTankBottomHtr.period;
    }

    uint32_t loxTankMidHtrSample() {
        sampleValve(&loxTankMidHtr);
        return loxTankMidHtr.period;
    }

    uint32_t loxTankTopHtrSample() {
        sampleValve(&loxTankTopHtr);
        return loxTankTopHtr.period;
    }

    // init function for valves namespace
    void initValves() {
        // link the right packet IDs to the valve open/close handler functions
        Comms::registerCallback(130, armValvePacketHandler);
        Comms::registerCallback(131, igniterPacketHandler);
        Comms::registerCallback(132, loxMainValvePacketHandler);
        Comms::registerCallback(133, fuelMainValvePacketHandler);

        Comms::registerCallback(135, loxTankBottomHtrPacketHandler);
        Comms::registerCallback(136, loxTankMidHtrPacketHandler);
        Comms::registerCallback(137, loxTankTopHtrPacketHandler);
    }

};