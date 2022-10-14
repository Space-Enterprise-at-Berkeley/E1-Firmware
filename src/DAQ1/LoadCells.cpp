#include "LoadCells.h"

namespace LoadCells {
    float loadCell0Value;
    float loadCell1Value;
    float loadCell2Value;
    float loadCell3Value;

    void initLoadCells() {
        HAL::lcAmp0.set_scale(3889); // measured imperically
        HAL::lcAmp1.set_scale(-3941); // measured imperically
        HAL::lcAmp2.set_scale(3889); // CHANGE ME
        HAL::lcAmp3.set_scale(3889); // CHANGE ME

        HAL::lcAmp0.tare();
        HAL::lcAmp1.tare();
        HAL::lcAmp2.tare();
        HAL::lcAmp3.tare();
    }

    uint32_t sampleLoadCells() {
        loadCell0Value = HAL::lcAmp0.get_units(); // in pounds
        loadCell1Value = HAL::lcAmp1.get_units(); // in pounds
        loadCell2Value = HAL::lcAmp2.get_units(); // in pounds
        loadCell3Value = HAL::lcAmp3.get_units(); // in pounds

        DEBUG("Load cell values: ");
        DEBUG(loadCell0Value);
        DEBUG(" : ");
        DEBUG(loadCell1Value);
        DEBUG(" : ");
        DEBUG(loadCell2Value);
        DEBUG(" : ");
        DEBUG(loadCell3Value);
        DEBUG("\n");

        Comms::Packet tmp = {.id = 116};
        Comms::packetAddFloat(&tmp, loadCell0Value);
        Comms::packetAddFloat(&tmp, loadCell1Value);
        Comms::packetAddFloat(&tmp, loadCell2Value);
        Comms::packetAddFloat(&tmp, loadCell3Value);
        Comms::packetAddFloat(&tmp, loadCell0Value + loadCell1Value + loadCell2Value + loadCell3Value);
        Comms::emitPacket(&tmp);

        return samplePeriod;
    }
};
