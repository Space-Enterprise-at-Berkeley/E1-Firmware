#include "Thermocouples.h"

namespace Thermocouples {
    uint32_t tcUpdatePeriod = 100 * 1000;
    Comms::Packet tcPacket = {.id = 20};

    float engineTC0Value;
    float engineTC1Value;
    float engineTC2Value;
    float engineTC3Value;

    float engineTC0ROC;
    float engineTC1ROC;
    float engineTC2ROC;
    float engineTC3ROC;


    float TC0ROCValues[10] = {0};
    float TC1ROCValues[10] = {0};
    float TC2ROCValues[10] = {0};
    float TC3ROCValues[10] = {0};

    void initThermocouples() {
    }

    uint32_t tcROCSample(float *thermocoupleValues, float currThermocoupleValue, float prevThermocoupleValue, float *ROCValue) {
        for (int i = 1; i < 10; i++) {
            thermocoupleValues[i] = thermocoupleValues[i-1];
        }
        thermocoupleValues[0] = (currThermocoupleValue - prevThermocoupleValue) / ((float)tcUpdatePeriod / 1e6);

        float sum = 0;
        for (int i = 0; i < 10; i++) {
            sum += thermocoupleValues[i];
        }
        *ROCValue = sum / 10;
    }

    uint32_t tcSample(MCP9600 *amp, uint8_t packetID, float *value, float *thermocoupleValues, float *ROCValue) {
        float reading = amp->readThermocouple();
        //calculate ROC TC value
        for (int i = 1; i < 10; i++) {
            thermocoupleValues[i] = thermocoupleValues[i-1];
        }
        thermocoupleValues[0] = (reading - *value) / ((float)tcUpdatePeriod / 1e6);

        float sum = 0;
        for (int i = 0; i < 10; i++) {
            sum += thermocoupleValues[i];
        }
        *ROCValue = sum / 10;
        
        // read from all TCs in sequence
        *value = reading;

        
        tcPacket.id = packetID;
        tcPacket.len = 0;
        Comms::packetAddFloat(&tcPacket, *value);
        
        Comms::emitPacket(&tcPacket);
        // return the next execution time
        return tcUpdatePeriod;
    }

    uint32_t tc0Sample() {
        return tcSample(&HAL::tcAmp0, 20, &engineTC0Value, TC0ROCValues, &engineTC0ROC); 
    }
    uint32_t tc1Sample() { 
        return tcSample(&HAL::tcAmp1, 21, &engineTC1Value, TC1ROCValues, &engineTC1ROC); 
    }
    uint32_t tc2Sample() {
        return tcSample(&HAL::tcAmp2, 22, &engineTC2Value, TC2ROCValues, &engineTC2ROC); 
    }
    uint32_t tc3Sample() { 
        return tcSample(&HAL::tcAmp3, 23, &engineTC3Value, TC3ROCValues, &engineTC3ROC); 
    }

};
