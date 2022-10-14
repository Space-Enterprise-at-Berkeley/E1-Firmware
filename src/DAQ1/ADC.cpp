#include "ADC.h"

namespace ADC {
    uint32_t adcUpdatePeriod = 100 * 1000;
    Comms::Packet adcPacket = {.id = 111};

    void initADC() {
    }

    float interpolate1000(uint16_t rawValue) {
        float tmp = (float) (rawValue - 6406);
        return tmp / 51.7;
    }

    uint32_t adcSample() {
        // read from all 8 ADC channels in sequence
        HAL::adc.readChannelOTF(0); // switch mux back to channel 0

        adcPacket.len = 0;
        Comms::packetAddFloat(&adcPacket, interpolate1000(HAL::adc.readChannelOTF(1)));
        Comms::packetAddFloat(&adcPacket, interpolate1000(HAL::adc.readChannelOTF(2)));
        Comms::packetAddFloat(&adcPacket, interpolate1000(HAL::adc.readChannelOTF(3)));
        Comms::packetAddFloat(&adcPacket, interpolate1000(HAL::adc.readChannelOTF(4)));
        Comms::packetAddFloat(&adcPacket, interpolate1000(HAL::adc.readChannelOTF(5)));
        Comms::packetAddFloat(&adcPacket, interpolate1000(HAL::adc.readChannelOTF(6)));
        Comms::packetAddFloat(&adcPacket, interpolate1000(HAL::adc.readChannelOTF(7))); // Fuel Dome PT
        Comms::packetAddFloat(&adcPacket, interpolate1000(HAL::adc.readChannelOTF(0)));

        Comms::emitPacket(&adcPacket);
        // return the next execution time
        return adcUpdatePeriod;
    }
};
