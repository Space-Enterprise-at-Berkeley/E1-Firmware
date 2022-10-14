#include "Power.h"

namespace Power {

    // TODO: change this to appropriate value
    uint32_t powerUpdatePeriod = 100 * 1000;

    Comms::Packet supply8Packet = {.id = 3};
    float supply8Voltage = 0.0;
    float supply8Current = 0.0;
    float supply8Power = 0.0;

    void initPower(){
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
