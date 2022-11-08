#include "Ducers.h"

namespace Ducers {
    uint32_t ptUpdatePeriod = 20 * 1000;
    Comms::Packet ptPacket = {.id = 10};

    float pressurantPTValue = 0.0;
    float loxTankPTValue = 0.0;
    float fuelTankPTValue = 0.0;
    float loxInjectorPTValue = 0.0;
    float fuelInjectorPTValue = 0.0;
    float loxDomePTValue = 0.0;
    float fuelDomePTValue = 0.0;

    void handleFastReadPacket(Comms::Packet tmp, uint8_t ip) {
        if(tmp.data[0]) {
            ptUpdatePeriod = 1 * 1000;
        } else {
            ptUpdatePeriod = 100 * 1000;
        }
    }

    void initDucers() {
        // Comms::registerCallback(140, handleFastReadPacket);
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
        // read from all 6 PTs in sequence
        
        HAL::adc1.readChannelOTF(1); // switch mux back to channel 1
        loxTankPTValue = interpolate1000(HAL::adc1.readChannelOTF(2)); // read channel 1, setup channel 2 for next read
        loxDomePTValue = interpolate1000(HAL::adc1.readChannelOTF(3)); // read channel 2, setup channel 3 for next read
        fuelTankPTValue = interpolate1000(HAL::adc1.readChannelOTF(4)); // read channel 3, setup channel 4 for next read
        fuelDomePTValue = interpolate1000(HAL::adc1.readChannelOTF(5)); // read channel 4, setup channel 5 for next read
        loxInjectorPTValue = (interpolate1000(HAL::adc1.readChannelOTF(6)) - 18.0)/ 1.02; // read channel 5, setup channel 6 for next read
        fuelInjectorPTValue = interpolate1000(HAL::adc1.readChannelOTF(0)); // read channel 6, reset mux to channel 1
        pressurantPTValue = interpolate5000(HAL::adc1.readChannelOTF(1));

        DEBUG("Read all PTs\n");
        DEBUG_FLUSH();

        // emit a packet with data
        ptPacket.len = 0;
        Comms::packetAddFloat(&ptPacket, loxTankPTValue);
        Comms::packetAddFloat(&ptPacket, fuelTankPTValue);
        Comms::packetAddFloat(&ptPacket, loxInjectorPTValue);
        Comms::packetAddFloat(&ptPacket, fuelInjectorPTValue);
        Comms::packetAddFloat(&ptPacket, pressurantPTValue);
        Comms::packetAddFloat(&ptPacket, loxDomePTValue);
        Comms::packetAddFloat(&ptPacket, fuelDomePTValue);

        Comms::emitPacket(&ptPacket);
        Comms::emitPacket(&ptPacket, &RADIO_SERIAL, "\r\n\n", 3);
        // return the next execution time
        DEBUG("PT Packet Sent\n");
        DEBUG_FLUSH();

        return ptUpdatePeriod;
    }

};
