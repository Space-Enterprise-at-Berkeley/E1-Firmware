#include <Valves.h>

namespace Valves {

    // TODO: change this to appropriate value
    uint32_t valveCheckPeriod = 50 * 1000;

    Comms::Packet armPacket = {.id = 5};
    float armVoltage = 0.0;
    float armCurrent = 0.0;

    Comms::Packet igniterPacket = {.id = 6};
    float igniterVoltage = 0.0;
    float igniterCurrent = 0.0;

    Comms::Packet loxMainPacket = {.id = 7};
    float loxMainVoltage = 0.0;
    float loxMainCurrent = 0.0;

    Comms::Packet fuelMainPacket = {.id = 8};
    float fuelMainVoltage = 0.0;
    float fuelMainCurrent = 0.0;

    void initValves() {
    }

    void sampleValve(Comms::Packet *packet, INA219 *ina, float *voltage, float *current) {
        *voltage = ina->readBusVoltage();
        *current = ina->readShuntCurrent();

        packet->len = 0;
        Comms::packetAddFloat(packet, *voltage);
        Comms::packetAddFloat(packet, *current);
        Comms::emitPacket(packet);
    }

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

};