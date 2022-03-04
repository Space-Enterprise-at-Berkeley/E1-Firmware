#include "BreakWire.h"

namespace BreakWire {

    Comms::Packet breakWirePacket = {.id = 39};

    void init() {
    }

    uint32_t sampleBreakWires() {
        float breakWire1Sample = scalingFactor * analogRead(HAL::breakWire1Pin);
        float breakWire2Sample = scalingFactor * analogRead(HAL::breakWire2Pin);

        DEBUG("BREAKWIRE 1 \t BREAKWIRE 2 \n");
        DEBUG(breakWire1Sample);
        DEBUG("\t");
        DEBUG(breakWire2Sample);
        DEBUG("\n");

        breakWirePacket.len = 0;
        Comms::packetAddFloat(&breakWirePacket, breakWire1Sample);
        Comms::packetAddFloat(&breakWirePacket, breakWire2Sample);
        Comms::emitPacket(&breakWirePacket);

        return samplingPeriod;
    }
}