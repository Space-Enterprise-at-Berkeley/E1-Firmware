#include "Power.h"

namespace Power {

    // TODO: change this to appropriate value
    uint32_t powerUpdatePeriod = 100 * 1000;

    Comms::Packet battPacket = {.id = 61};
    float battVoltage = 0.0;
    float battCurrent = 0.0;
    float battPower = 0.0;

    Comms::Packet supply12Packet = {.id = 62};
    float supply12Voltage = 0.0;
    float supply12Current = 0.0;
    float supply12Power = 0.0;

    void initPower(){
    }

    uint32_t battSample() {
        // Reads current, voltage, power values for battery supply
        battVoltage = HAL::supplyBatt.readBusVoltage();
        battCurrent = HAL::supplyBatt.readShuntCurrent();
        battPower = battVoltage * battCurrent;

        battPacket.len = 0;
        Comms::packetAddFloat(&battPacket, battVoltage);
        Comms::packetAddFloat(&battPacket, battCurrent);
        Comms::packetAddFloat(&battPacket, battPower);
        Comms::emitPacket(&battPacket);

        return powerUpdatePeriod;
    }

    uint32_t supply12Sample() {
        supply12Voltage = HAL::supply12v.readBusVoltage();
        supply12Current = HAL::supply12v.readShuntCurrent();
        supply12Power = supply12Voltage * supply12Current;

        supply12Packet.len = 0;
        Comms::packetAddFloat(&supply12Packet, supply12Voltage);
        Comms::packetAddFloat(&supply12Packet, supply12Current);
        Comms::packetAddFloat(&supply12Packet, supply12Power);
        Comms::emitPacket(&supply12Packet);

        return powerUpdatePeriod;
    }

}
