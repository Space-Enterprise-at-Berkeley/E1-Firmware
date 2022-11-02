#include "Ducers.h"

namespace Ducers {
    uint32_t ptUpdatePeriod = 100 * 1000;
    Comms::Packet ptPacket = {.id = 81};

    float rqdPTValue = 0.0;
    float purgePTValue = 0.0;

    void handleFastReadPacket(Comms::Packet tmp, uint8_t ip) {
        if(tmp.data[0]) {
            ptUpdatePeriod = 1 * 1000;
        } else {
            ptUpdatePeriod = 100 * 1000;
        }
    }

    void initDucers() {
    }

    float interpolate1000(uint16_t rawValue) {
        float tmp = (float) (rawValue - 6406);
        return tmp / 51.7;
    }

    float interpolate5000(uint16_t rawValue) {
        float tmp = (float) rawValue;
        return tmp / 12.97;
    }

    uint32_t ptSample() {

        rqdPTValue = interpolate1000(HAL::adc2.readChannelOTF(6)); // read channel 1, setup channel 2 for next read
        purgePTValue = interpolate1000(HAL::adc2.readChannelOTF(5)); // read channel 2, setup channel 3 for next read
        // mainValvePTValue = interpolate1000(HAL::adc2.readChannelOTF(6));

        DEBUG("Read all PTs\n");
        DEBUG_FLUSH();

        // emit a packet with data
        ptPacket.len = 0;
        Comms::packetAddFloat(&ptPacket, rqdPTValue);
        Comms::packetAddFloat(&ptPacket, purgePTValue);

        Comms::emitPacket(&ptPacket);
        // return the next execution time
        DEBUG("PT Packet Sent\n");
        DEBUG_FLUSH();

        return ptUpdatePeriod;
    }

};
