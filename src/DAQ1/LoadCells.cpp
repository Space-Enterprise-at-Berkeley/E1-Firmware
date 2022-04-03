#include "LoadCells.h"

namespace LoadCells {
    float loadCell1Value;
    float loadCell2Value;

    void initLoadCells() {
        HAL::lcAmp0.set_scale(3889); // measured imperically
        HAL::lcAmp1.set_scale(-3941); // measured imperically

        HAL::lcAmp0.tare();
        HAL::lcAmp1.tare();
    }

    uint32_t sampleLoadCells() {
        loadCell1Value = HAL::lcAmp0.get_units(); // in pounds
        loadCell2Value = HAL::lcAmp1.get_units(); // in pounds

        Comms::Packet tmp = {.id = 120};
        Comms::packetAddFloat(&tmp, loadCell1Value);
        Comms::packetAddFloat(&tmp, loadCell2Value);
        Comms::packetAddFloat(&tmp, loadCell1Value + loadCell2Value);
        Comms::emitPacket(&tmp);
        Comms::emitPacket(&tmp, 42); // send this packet to the flight computer as well

        return samplePeriod;
    }
};
