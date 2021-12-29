#include <Valves.h>

namespace Valves {

    // TODO: change this to appropriate value
    uint32_t valveCheckPeriod = 50 * 1000;

    // state of each valve is stored in each bit
    // bit 0 is armValve, bit 1 is igniter, etc
    // the order of bits is the same as the order of valves on the E-1 Design spreadsheet
    uint8_t valveStates = 0x00;

    // info for each individual valve
    Comms::Packet armPacket = {.id = 30};
    float armVoltage = 0.0;
    float armCurrent = 0.0;

    Comms::Packet igniterPacket = {.id = 31}; // note that even though the igniter isn't a valve, we kinda just count it as one
    float igniterVoltage = 0.0;
    float igniterCurrent = 0.0;

    Comms::Packet loxMainPacket = {.id = 32};
    float loxMainVoltage = 0.0;
    float loxMainCurrent = 0.0;

    Comms::Packet fuelMainPacket = {.id = 33};
    float fuelMainVoltage = 0.0;
    float fuelMainCurrent = 0.0;

    uint8_t OClimits[4] = {1,1,1,1}; //(amps): Arming valve, igniter, lox main valve, fuel main valve
    // common function for opening a valve
    void openValve(uint8_t pin, uint8_t valveID) {
        digitalWriteFast(pin, HIGH); // turn on the physical pin
        valveStates |= (0x01 << valveID); // set bit <valveID> to 1

        Comms::Packet tmp = {.id = valveID + 40}; // valve packets have an offset of 40 (check the E-1 Design spreadsheet)
        Comms::packetAddUint8(&tmp, 1); // a value of 1 indicates the valve was turned on
        Comms::emitPacket(&tmp);
        
        tmp.id = 49; // reuse tmp to send status of all valves
        tmp.len = 0;
        Comms::packetAddUint8(&tmp, valveStates);
        Comms::emitPacket(&tmp);
    }

    // common function for closing a valve
    void closeValve(uint8_t pin, uint8_t valveID, uint8_t overcurrentShutoff = 0) { //optional argument overcurrentShutoff
        digitalWriteFast(pin, LOW); // turn off the physical pin
        valveStates &= ~(0x01 << valveID); // set bit <valveID> to 1

        Comms::Packet tmp = {.id = valveID + 40}; // valve packets have an offset of 40 (check the E-1 Design spreadsheet)
        Comms::packetAddUint8(&tmp, 0 + 2*overcurrentShutoff); // a value of 0 indicates the valve was turned off, 2 indicates overcurrent shutoff
        Comms::emitPacket(&tmp);
        
        tmp.id = 49; // reuse tmp to send status of all valves
        tmp.len = 0;
        Comms::packetAddUint8(&tmp, valveStates);
        Comms::emitPacket(&tmp);
    }

    // functions for each individual valve
    void openArmValve() { openValve(armPin, 0); }
    void closeArmValve(int OCShutoff = 0) { closeValve(armPin, 0, OCShutoff); }
    void armValvePacketHandler(Comms::Packet tmp) { return tmp.data[0] ? openArmValve() : closeArmValve(); }

    void activateIgniter() { openValve(igniterPin, 1); }
    void deactivateIgniter(int OCShutoff = 0) { closeValve(igniterPin, 1, OCShutoff); }
    void igniterPacketHandler(Comms::Packet tmp) { return tmp.data[0] ? activateIgniter() : deactivateIgniter(); }

    void openLoxMainValve() { openValve(loxMainPin, 2); }
    void closeLoxMainValve(int OCShutoff = 0) { closeValve(loxMainPin, 2, OCShutoff); }
    void loxMainValvePacketHandler(Comms::Packet tmp) { return tmp.data[0] ? openLoxMainValve() : closeLoxMainValve(); }

    void openFuelMainValve() { openValve(fuelMainPin, 3); }
    void closeFuelMainValve(int OCShutoff = 0) { closeValve(fuelMainPin, 3, OCShutoff); }
    void fuelMainValvePacketHandler(Comms::Packet tmp) { return tmp.data[0] ? openFuelMainValve() : closeFuelMainValve(); }

    // common function for sampling a valve's voltage and current
    void sampleValve(Comms::Packet *packet, INA219 *ina, float *voltage, float *current) {
        *voltage = ina->readBusVoltage();
        *current = ina->readShuntCurrent();
        
        uint8_t id = packet->id; //packet IDs: 30 -> 33
        float threshold = OClimits[(id - 30)];
        //IDS: Arming valve, igniter, lox main valve, fuel main valve
        if (*current > threshold) {
            switch ((id - 30)) {
                case 0: closeArmValve(1); break; 
                case 1: deactivateIgniter(1); break;
                case 2: closeLoxMainValve(1); break;
                case 3: closeFuelMainValve(1); break;
            }
        }

        packet->len = 0;
        Comms::packetAddFloat(packet, *voltage);
        Comms::packetAddFloat(packet, *current);
        Comms::emitPacket(packet);
    }

    // individual task functions (see taskTable in main.cpp)
    uint32_t armValveSample() {
        sampleValve(&armPacket, &HAL::chan0, &armVoltage, &armCurrent);
        return valveCheckPeriod;
    }

    uint32_t igniterSample() {
        sampleValve(&igniterPacket, &HAL::chan1, &igniterVoltage, &igniterCurrent);
        return valveCheckPeriod;
    }

    uint32_t loxMainValveSample() {
        sampleValve(&loxMainPacket, &HAL::chan2, &loxMainVoltage, &loxMainCurrent);
        return valveCheckPeriod;
    }

    uint32_t fuelMainValveSample() {
        sampleValve(&fuelMainPacket, &HAL::chan3, &fuelMainVoltage, &fuelMainCurrent);
        return valveCheckPeriod;
    }

    // init function for valves namespace
    void initValves() {
        // link the right packet IDs to the valve open/close handler functions
        Comms::registerCallback(50, armValvePacketHandler);
        Comms::registerCallback(51, igniterPacketHandler);
        Comms::registerCallback(52, loxMainValvePacketHandler);
        Comms::registerCallback(53, fuelMainValvePacketHandler);
    }

};