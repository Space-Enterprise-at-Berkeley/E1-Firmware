#include <Power.h>

namespace Power {

    // TODO: change this to appropriate value
    uint32_t powerUpdatePeriod = 100 * 1000;

    Comms::Packet battPacket = {.id = 1};
    float battVoltage = 0.0;
    float battCurrent = 0.0;
    float battPower = 0.0;

    Comms::Packet supply12Packet = {.id = 2};
    float supply12Voltage = 0.0;
    float supply12Current = 0.0;
    float supply12Power = 0.0;

    Comms::Packet supply8Packet = {.id = 3};
    float supply8Voltage = 0.0;
    float supply8Current = 0.0;
    float supply8Power = 0.0;

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

    uint32_t supply8Sample() {
        supply8Voltage = HAL::supply8v.readBusVoltage();
        supply8Current = HAL::supply8v.readShuntCurrent();
        supply8Power = supply8Voltage * supply8Current;

        supply8Packet.len = 0;
        Comms::packetAddFloat(&supply8Packet, supply8Voltage);
        Comms::packetAddFloat(&supply8Packet, supply8Current);
        Comms::packetAddFloat(&supply8Packet, supply8Power);
        Comms::emitPacket(&supply8Packet);

        return powerUpdatePeriod;
    }
}
