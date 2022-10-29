#include "Power.h"

namespace Power {

    // TODO: change this to appropriate value
    uint32_t powerUpdatePeriod = 100 * 1000;

    Comms::Packet supply8Packet = {.id = 3};
    float supply8Voltage = 0.0;
    float supply8Current = 0.0;
    float supply8Power = 0.0;

    uint32_t supply8Sample() {
        DEBUG("pwr\n");
        DEBUG_FLUSH();
        supply8Voltage = HAL::supply8v.readBusVoltage();
        DEBUG("pwr1\n");
        DEBUG_FLUSH();
        supply8Current = HAL::supply8v.readShuntCurrent();
        DEBUG("pwr2\n");
        DEBUG_FLUSH();
        supply8Power = supply8Voltage * supply8Current;
        supply8Packet.len = 0;
        Comms::packetAddFloat(&supply8Packet, supply8Voltage);
        Comms::packetAddFloat(&supply8Packet, supply8Current);
        Comms::packetAddFloat(&supply8Packet, supply8Power);
        Comms::emitPacket(&supply8Packet);
        DEBUG("pwr done\n");
        DEBUG_FLUSH();
        return powerUpdatePeriod;
    }
}
