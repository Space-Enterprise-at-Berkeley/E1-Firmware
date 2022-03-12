#include "BreakWire.h"

namespace BreakWire {

    Comms::Packet breakWirePacket = {.id = 39};

    void init() {
        Comms::registerEmitter({.packet = &breakWirePacket, .updatePeriod = samplingPeriod});
        BlackBox::registerEmitter({.packet = &breakWirePacket, .updatePeriod = samplingPeriod});
    }

    uint32_t sampleBreakWires() {
        float breakWire1Sample = scalingFactor * analogRead(HAL::breakWire1Pin);
        float breakWire2Sample = scalingFactor * analogRead(HAL::breakWire2Pin);

        breakWirePacket.len = 0;
        Comms::packetAddFloat(&breakWirePacket, breakWire1Sample);
        Comms::packetAddFloat(&breakWirePacket, breakWire2Sample);

        return samplingPeriod;
    }
}