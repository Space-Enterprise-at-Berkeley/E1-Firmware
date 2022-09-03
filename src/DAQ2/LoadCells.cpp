#include "LoadCells.h"

namespace LoadCells {
    float loadCell1Value;
    float loadCell2Value;

    void initLoadCells() {
        HAL::lcAmp1.set_scale(3989); // measured 03/12/22
        HAL::lcAmp2.set_scale(3995); // measured 03/12/22

        HAL::lcAmp1.tare();
        HAL::lcAmp2.tare();
    }

    uint32_t sampleLoadCells() {

        DEBUG("READING LOAD CELLS");
        loadCell1Value = HAL::lcAmp1.get_units(); // in pounds
        loadCell2Value = HAL::lcAmp2.get_units(); // in pounds

        DEBUG(loadCell1Value);
        DEBUG(";");
        DEBUG(loadCell2Value);
        DEBUG("\n");

        Comms::Packet tmp = {.id = 120};
        Comms::packetAddFloat(&tmp, loadCell1Value);
        Comms::packetAddFloat(&tmp, loadCell2Value);
        Comms::packetAddFloat(&tmp, loadCell1Value + loadCell2Value);
        Comms::emitPacket(&tmp);
        Comms::emitPacket(&tmp, 43); // send this packet to the flight computer as well

        return samplePeriod;
    }
};
