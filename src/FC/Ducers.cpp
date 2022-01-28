#include "Ducers.h"

namespace Ducers {
    uint32_t ptUpdatePeriod = 100 * 1000;
    Comms::Packet ptPacket = {.id = 10};

    float prevPressurantPTValue = 0.0;

    float pressurantPTValue = 0.0;
    float loxTankPTValue = 0.0;
    float fuelTankPTValue = 0.0;
    float loxInjectorPTValue = 0.0;
    float fuelInjectorPTValue = 0.0;
    float loxDomePTValue = 0.0;
    float fuelDomePTValue = 0.0;

    float pressurantPTROC = 0.0;

    void handleFastReadPacket(Comms::Packet tmp) {
        if(tmp.data[0]) {
            ptUpdatePeriod = 1 * 1000;
        } else {
            ptUpdatePeriod = 100 * 1000;
        }
    }

    void initDucers() {
        Comms::registerCallback(140, handleFastReadPacket);
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
        HAL::adc1.readChannelOTF(0); // switch mux back to channel 0
        loxTankPTValue = interpolate1000(HAL::adc1.readChannelOTF(1)); 
        fuelTankPTValue = interpolate1000(HAL::adc1.readChannelOTF(2)); // read channel 1, setup channel 2 for next read
        loxInjectorPTValue = interpolate1000(HAL::adc1.readChannelOTF(3)); // read channel 2, setup channel 3 for next read
        fuelInjectorPTValue = interpolate1000(HAL::adc1.readChannelOTF(0)); // read channel 3, reset mux to channel 0
        HAL::adc2.readChannelOTF(4); // switch mux back to channel 4
        pressurantPTValue = interpolate5000(HAL::adc2.readChannelOTF(5)); // read channel 4, setup channel 5 for next read
        loxDomePTValue = interpolate1000(HAL::adc2.readChannelOTF(0)); // read channel 5, reset mux to channel 0

        // emit a packet with data
        ptPacket.len = 0;
        Comms::packetAddFloat(&ptPacket, loxTankPTValue);
        Comms::packetAddFloat(&ptPacket, fuelTankPTValue);
        Comms::packetAddFloat(&ptPacket, loxInjectorPTValue);
        Comms::packetAddFloat(&ptPacket, fuelInjectorPTValue);
        Comms::packetAddFloat(&ptPacket, pressurantPTValue);
        Comms::packetAddFloat(&ptPacket, loxDomePTValue);

        Comms::emitPacket(&ptPacket);
        // return the next execution time
        return ptUpdatePeriod;
    }

    uint32_t pressurantPTROCSample() {
        pressurantPTROC = (pressurantPTValue - prevPressurantPTValue) / 1e6;
        prevPressurantPTValue = pressurantPTValue;
        return 1e6;
    }
};
