#include "LoadCells.h"

namespace LoadCells {
    float loadCell1Value;
    float loadCell2Value;

    void initLoadCells() {}

    uint32_t sampleLoadCells() {
        loadCell1Value = HAL::lcAmp0.read();
        loadCell2Value = HAL::lcAmp1.read();

        Comms::Packet tmp = {.id = 120};
        Comms::packetAddFloat(&tmp, loadCell1Value);
        Comms::packetAddFloat(&tmp, loadCell2Value);
        Comms::packetAddFloat(&tmp, loadCell1Value + loadCell2Value);
        Comms::emitPacket(&tmp);

        return samplePeriod;
    }
};
