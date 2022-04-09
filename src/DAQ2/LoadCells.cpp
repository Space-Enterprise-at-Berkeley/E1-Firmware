#include "LoadCells.h"

namespace LoadCells {
    float loadCell1Value;
    float loadCell2Value;

    void initLoadCells() {

        HAL::lcAmp3.set_scale(3989); // measured 03/12/22
        HAL::lcAmp4.set_scale(3995); // measured 03/12/22

        HAL::lcAmp3.tare();
        HAL::lcAmp4.tare();
    }

    uint32_t sampleLoadCells() {

        DEBUG("READING LOAD CELLS");
        loadCell1Value = -1 * HAL::lcAmp3.get_units(); // in pounds
        loadCell2Value = HAL::lcAmp4.get_units(); // in pounds

        DEBUG(loadCell1Value);
        DEBUG(";");
        DEBUG(loadCell2Value);
        DEBUG("\n");

        Comms::Packet tmp = {.id = 120};
        Comms::packetAddFloat(&tmp, loadCell1Value);
        Comms::packetAddFloat(&tmp, loadCell2Value);
        Comms::packetAddFloat(&tmp, loadCell1Value + loadCell2Value);
        Comms::emitPacket(&tmp);
        Comms::emitPacket(&tmp, 42); // send this packet to the flight computer as well

        return samplePeriod;
    }
};
