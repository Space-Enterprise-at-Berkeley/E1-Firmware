#include "Thermocouples.h"

namespace Thermocouples {
    uint32_t tcUpdatePeriod = 100 * 1000;
    Comms::Packet tcPacket;

    float TC0Value;
    float TC1Value;
    float TC2Value;
    float TC3Value;

    void initThermocouples() {
    }

    uint32_t tcSample(MCP9600 *amp, uint8_t packetID, float *value) {
        // read from all TCs in sequence
        *value = amp->readThermocouple();
        
        tcPacket.id = packetID;
        tcPacket.len = 0;
        Comms::packetAddFloat(&tcPacket, *value);
        
        Comms::emitPacket(&tcPacket, 43);
        Comms::emitPacket(&tcPacket);
        // return the next execution time
        return tcUpdatePeriod;
    }

    uint32_t tc0Sample() { return tcSample(&HAL::tcAmp0, 112, &TC0Value); }
    uint32_t tc1Sample() { return tcSample(&HAL::tcAmp1, 113, &TC1Value); }
    uint32_t tc2Sample() { return tcSample(&HAL::tcAmp2, 114, &TC2Value); }
    uint32_t tc3Sample() { return tcSample(&HAL::tcAmp3, 115, &TC3Value); }

};
