#include "Thermocouples.h"

namespace Thermocouples {
    uint32_t tcUpdatePeriod = 100 * 1000;
    Comms::Packet tcPacket = {.id = 20};

    float engineTC0Value;
    float engineTC1Value;
    float engineTC2Value;
    float engineTC3Value;

    void initThermocouples() {
    }

    uint32_t tcSample(MCP9600 *amp, uint8_t packetID, float *value) {
        // read from all TCs in sequence
        *value = amp->readThermocouple();
        
        tcPacket.id = packetID;
        tcPacket.len = 0;
        Comms::packetAddFloat(&tcPacket, *value);
        
        Comms::emitPacket(&tcPacket);
        // return the next execution time
        return tcUpdatePeriod;
    }

    uint32_t tc0Sample() { return tcSample(&HAL::tcAmp0, 20, &engineTC0Value); }
    uint32_t tc1Sample() { return tcSample(&HAL::tcAmp1, 21, &engineTC1Value); }
    uint32_t tc2Sample() { return tcSample(&HAL::tcAmp2, 22, &engineTC2Value); }
    uint32_t tc3Sample() { return tcSample(&HAL::tcAmp3, 23, &engineTC3Value); }

};
