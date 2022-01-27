#include "Ducers.h"

namespace Ducers {
    uint32_t ptUpdatePeriod = 100 * 1000;
    Comms::Packet ptPacket = {.id = 10};

    float pressurantPTValue = 0.0;
    float loxTankPTValue = 0.0;
    float fuelTankPTValue = 0.0;
    float loxInjectorPTValue = 0.0;
    float fuelInjectorPTValue = 0.0;
    float loxDomePTValue = 0.0;
    float fuelDomePTValue = 0.0;

    float pressurantPTROC;
    float loxTankPTROC;
    float fuelTankPTROC;
    float loxInjectorPTROC;
    float fuelInjectorPTROC;
    float loxDomePTROC;
    float fuelDomePTROC;

    float pressurantPTROCValues[10] = {0};
    float loxTankPTROCValues[10] = {0};
    float fuelTankPTROCValues[10] = {0};
    float loxInjectorPTROCValues[10] = {0};
    float fuelInjectorPTROCValues[10] = {0};
    float loxDomePTROCValues[10] = {0};
    float fuelDomePTROCValues[10] = {0};

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
        // loxTankPTValue = interpolate1000(HAL::adc1.readChannelOTF(1)); 
        // fuelTankPTValue = interpolate1000(HAL::adc1.readChannelOTF(2)); // read channel 1, setup channel 2 for next read
        // loxInjectorPTValue = interpolate1000(HAL::adc1.readChannelOTF(3)); // read channel 2, setup channel 3 for next read
        // fuelInjectorPTValue = interpolate1000(HAL::adc1.readChannelOTF(0)); // read channel 3, reset mux to channel 0
        ptSampleHelper(&loxTankPTValue, &loxTankPTROC, loxTankPTValue, 1, loxTankPTROCValues);
        ptSampleHelper(&fuelTankPTValue, &fuelTankPTROC, fuelTankPTValue, 2, fuelTankPTROCValues);
        ptSampleHelper(&loxInjectorPTValue, &loxInjectorPTROC, loxInjectorPTValue, 3, loxInjectorPTROCValues);
        ptSampleHelper(&fuelInjectorPTValue, &fuelInjectorPTROC, fuelInjectorPTValue, 0, fuelInjectorPTROCValues);
        HAL::adc2.readChannelOTF(4); // switch mux back to channel 4
        // pressurantPTValue = interpolate5000(HAL::adc2.readChannelOTF(5)); // read channel 4, setup channel 5 for next read
        // loxDomePTValue = interpolate1000(HAL::adc2.readChannelOTF(0)); // read channel 5, reset mux to channel 0
        ptSampleHelper(&pressurantPTValue, &pressurantPTROC, pressurantPTValue, 5, pressurantPTROCValues);
        ptSampleHelper(&loxDomePTValue, &loxDomePTROC, loxDomePTValue, 0, loxDomePTROCValues);
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

    /**
     * @brief updates both the PT value and the PT ROC value
     * 
     * @param value PtValue to be updated
     * @param ROCValue ROC value to be updated
     * @param prevValue the pt value before the most current reading
     * @param channel channel next sensor is on
     * @param ptValues array of previous ROC values
     */
    void ptSampleHelper(float *value, float *ROCValue, float prevValue, int channel, float* ptValues) {
        float reading = HAL::adc1.readChannelOTF(channel);
        //update ROC value
        ptROCSample(ptValues, reading, prevValue, ROCValue);
        *value = reading;
    }

    void ptROCSample(float *ptValues, float currPTValue, float prevPTValue, float *ROCValue) {
        for (int i = 1; i < 10; i++) {
            ptValues[i] = ptValues[i-1];
        }
        ptValues[0] = (currPTValue - prevPTValue) / ((float)ptUpdatePeriod / 1e6);

        float sum = 0;
        for (int i = 0; i < 10; i++) {
            sum += ptValues[i];
        }
        *ROCValue = sum / 10;
    }
};
