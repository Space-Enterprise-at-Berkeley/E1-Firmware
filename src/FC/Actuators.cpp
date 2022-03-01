#include "Actuators.h"

namespace Actuators {
    // state of each valve is stored in each bit
    // bit 0 is armValve, bit 1 is igniter, etc
    // the order of bits is the same as the order of valves on the E-1 Design spreadsheet
    uint8_t valveStates = 0x00; //TODO - change to uint16_t, since there are more than 8 power channels on FC

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

    Valve breakWire = {.valveID = 255, // break wire can't be actuated, no valveID is used. 
                      .statePacketID = 0,
                      .statusPacketID = 34,
                      .pin = HAL::chan5Pin,
                      .expanderPin = 255,
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 0.1,
                      .period = 50 * 1000,
                      .ina = &HAL::chan5};
    
    Valve loxTankBottomHtr = {.valveID = 5, // actuated from the IO Expander
                      .statePacketID = 45,
                      .statusPacketID = 35,
                      .pin = 255, // dont use pin
                      .expanderPin = HAL::chan7Pin,
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 3.0,
                      .period = 50 * 1000,
                      .ina = &HAL::chan7};

    Valve loxTankMidHtr = {.valveID = 6, // actuated from the IO Expander
                      .statePacketID = 46,
                      .statusPacketID = 36,
                      .pin = 255, // dont use pin
                      .expanderPin = HAL::chan8Pin,
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 3.0,
                      .period = 50 * 1000,
                      .ina = &HAL::chan8};

    Valve loxTankTopHtr = {.valveID = 7, // actuated from the IO Expander
                      .statePacketID = 47,
                      .statusPacketID = 37,
                      .pin = 255, // dont use pin
                      .expanderPin = HAL::chan9Pin,
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 3.0,
                      .period = 50 * 1000,
                      .ina = &HAL::chan9};
  
    Valve igniterEnableRelay = {.valveID = 4, // actuated from the IO Expander
                      .statePacketID = 48,
                      .statusPacketID = 38,
                      .pin = 255, 
                      .expanderPin = HAL::chan10Pin,
                      .voltage = 0.0,
                      .current = 0.0,
                      .ocThreshold = 3.0,
                      .period = 100 * 1000,
                      .ina = &HAL::chan10};

    //info for each input on the mux
    MuxActuator chute1 = {.statusPacketID = -1, //TODO make actual value
                      .channel = 0,
                      .voltage = 0.0,
                      .current = 0.0,
                      .period = 50 * 1000};

    MuxActuator chute2 = {.statusPacketID = -1, //TODO make actual value
                      .channel = 1,
                      .voltage = 0.0,
                      .current = 0.0,
                      .period = 50 * 1000};

    MuxActuator cam1 = {.statusPacketID = -1, //TODO make actual value
                      .channel = 2,
                      .voltage = 0.0,
                      .current = 0.0,
                      .period = 50 * 1000};
    
    MuxActuator rfAmp = {.statusPacketID = -1, //TODO make actual value
                      .channel = 3,
                      .voltage = 0.0,
                      .current = 0.0,
                      .period = 50 * 1000};

    MuxActuator cam2 = {.statusPacketID = -1, //TODO make actual value
                      .channel = 4,
                      .voltage = 0.0,
                      .current = 0.0,
                      .period = 50 * 1000};

    MuxActuator radio = {.statusPacketID = -1, //TODO make actual value
                      .channel = 5,
                      .voltage = 0.0,
                      .current = 0.0,
                      .period = 50 * 1000};

    MuxActuator valve1 = {.statusPacketID = -1, //TODO make actual value
                      .channel = 7,
                      .voltage = 0.0,
                      .current = 0.0,
                      .period = 50 * 1000};

    MuxActuator valve2 = {.statusPacketID = -1, //TODO make actual value
                      .channel = 8,
                      .voltage = 0.0,
                      .current = 0.0,
                      .period = 50 * 1000};

    MuxActuator valve3 = {.statusPacketID = -1, //TODO make actual value
                      .channel = 9,
                      .voltage = 0.0,
                      .current = 0.0,
                      .period = 50 * 1000};

    MuxActuator valve4 = {.statusPacketID = -1, //TODO make actual value
                      .channel = 10,
                      .voltage = 0.0,
                      .current = 0.0,
                      .period = 50 * 1000};

    MuxActuator valve5 = {.statusPacketID = -1, //TODO make actual value
                      .channel = 11,
                      .voltage = 0.0,
                      .current = 0.0,
                      .period = 50 * 1000};

    MuxActuator valve6 = {.statusPacketID = -1, //TODO make actual value
                      .channel = 12,
                      .voltage = 0.0,
                      .current = 0.0,
                      .period = 50 * 1000};

    MuxActuator hBridge1 = {.statusPacketID = -1, //TODO make actual value
                      .channel = 13,
                      .voltage = 0.0,
                      .current = 0.0,
                      .period = 50 * 1000};

    MuxActuator hBridge2 = {.statusPacketID = -1, //TODO make actual value
                      .channel = 14,
                      .voltage = 0.0,
                      .current = 0.0,
                      .period = 50 * 1000};

    MuxActuator hBridge3 = {.statusPacketID = -1, //TODO make actual value
                      .channel = 15,
                      .voltage = 0.0,
                      .current = 0.0,
                      .period = 50 * 1000};

    MuxActuator break1 = {.statusPacketID = -1, //TODO make actual value
                      .channel = 5,
                      .voltage = 0.0,
                      .current = 0.0,
                      .period = 50 * 1000};

    MuxActuator break2 = {.statusPacketID = -1, //TODO make actual value
                      .channel = 6,
                      .voltage = 0.0,
                      .current = 0.0,
                      .period = 50 * 1000};
    
    


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

    void enableIgniter() { openValve(&igniterEnableRelay); }
    void disableIgniter(uint8_t OCShutoff = 0) { closeValve(&igniterEnableRelay, OCShutoff); }
    void igniterEnableRelayPacketHandler(Comms::Packet tmp) { return tmp.data[0] ? enableIgniter() : disableIgniter(); }

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

    uint32_t breakWireSample() {
        sampleValve(&breakWire);
        return breakWire.period;
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

    uint32_t igniterEnableRelaySample() {
        sampleValve(&igniterEnableRelay);
        return igniterEnableRelay.period;
    }

    // init function for valves namespace
    void initActuators() {
        // link the right packet IDs to the valve open/close handler functions
        Comms::registerCallback(130, armValvePacketHandler);
        Comms::registerCallback(131, igniterPacketHandler);
        Comms::registerCallback(132, loxMainValvePacketHandler);
        Comms::registerCallback(133, fuelMainValvePacketHandler);

        Comms::registerCallback(135, loxTankBottomHtrPacketHandler);
        Comms::registerCallback(136, loxTankMidHtrPacketHandler);
        Comms::registerCallback(137, loxTankTopHtrPacketHandler);

        Comms::registerCallback(138, igniterEnableRelayPacketHandler);
    }

    void sampleMuxActuator(MuxActuator *muxActuator) {
        //set mux select
        digitalWrite(muxSelect1Pin, muxActuator->channel & 1);
        digitalWrite(muxSelect2Pin, muxActuator->channel >> 1 & 1);
        digitalWrite(muxSelect3Pin, muxActuator->channel >> 2 & 1);
        digitalWrite(muxSelect4Pin, muxActuator->channel >> 3 & 1);

        muxActuator->continuity = digitalRead(muxContinuityPin);

        int currentReading = analogRead(muxCurrentPin);
        //TODO figure out how to convert reading to current/voltage

        //TODO for nonvalves one of the values will be garbage :(
        Comms::Packet packet = {.id = muxActuator->statusPacketID};
        Comms::packetAddFloat(&packet, muxActuator->voltage);
        Comms::packetAddFloat(&packet, muxActuator->current);
        Comms::packetAddUint8(&packet, muxActuator->continuity);
        Comms::emitPacket(&packet);
    }

    uint32_t chute1Sample() {
        sampleMuxActuator(&chute1);
        return chute1.period;
    }

    uint32_t chute2Sample() {
        sampleMuxActuator(&chute2);
        return chute2.period;
    }

    uint32_t cam1Sample() {
        sampleMuxActuator(&cam1);
        return cam1.period;
    }

    uint32_t rfAmpSample() {
        sampleMuxActuator(&rfAmp);
        return rfAmp.period;
    }

    uint32_t cam2Sample() {
        sampleMuxActuator(&cam2);
        return cam2.period;
    }

    uint32_t radioSample() {
        sampleMuxActuator(&radio);
        return radio.period;
    }

    uint32_t valve1Sample() {
        sampleMuxActuator(&valve1);
        return valve1.period;
    }

    uint32_t valve2Sample() {
        sampleMuxActuator(&valve2);
        return valve2.period;
    }

    uint32_t valve3Sample() {
        sampleMuxActuator(&valve3);
        return valve3.period;
    }

    uint32_t valve4Sample() {
        sampleMuxActuator(&valve4);
        return valve4.period;
    }

    uint32_t valve5Sample() {
        sampleMuxActuator(&valve5);
        return valve5.period;
    }

    uint32_t valve6Sample() {
        sampleMuxActuator(&valve6);
        return valve6.period;
    }

    uint32_t hBridge1Sample() {
        sampleMuxActuator(&hBridge1);
        return hBridge1.period;
    }

    uint32_t hBridge2Sample() {
        sampleMuxActuator(&hBridge2);
        return hBridge2.period;
    }

    uint32_t hBridge3Sample() {
        sampleMuxActuator(&hBridge3);
        return hBridge3.period;
    }

    uint32_t break1Sample() {
        sampleMuxActuator(&break1);
        return break1.period;
    }

    uint32_t break2Sample() {
        sampleMuxActuator(&break2);
        return break2.period;
    }

};