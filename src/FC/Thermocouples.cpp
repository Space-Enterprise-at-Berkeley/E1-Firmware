#include "Thermocouples.h"

namespace Thermocouples {
    uint32_t tcUpdatePeriod = 100 * 1000;
    Comms::Packet tcPacket = {.id = 20};

    void initThermocouples() {
    }

    uint32_t tcSample(MCP9600 *amp, uint8_t packetID) {
        // uint32_t t1 = micros();
        float reading = amp->readThermocouple();
        // uint32_t t2 = micros(); 

        // DEBUG("time: ");
        // DEBUG(t2 - t1);
        // DEBUG("\n");
        // DEBUG_FLUSH();

        tcPacket.id = packetID;
        tcPacket.len = 0;
        Comms::packetAddFloat(&tcPacket, reading);
        
        Comms::emitPacket(&tcPacket);
        Comms::emitPacket(&tcPacket, &RADIO_SERIAL, "\r\n\n", 3);
        // return the next execution time
        return tcUpdatePeriod;
    }

    uint32_t tc0Sample() {
        return tcSample(&HAL::tcAmp0, 20); 
    }
    uint32_t tc1Sample() { 
        return tcSample(&HAL::tcAmp1, 21); 
    }
    uint32_t tc2Sample() {
        return tcSample(&HAL::tcAmp2, 22); 
    }
    uint32_t tc3Sample() { 
        return tcSample(&HAL::tcAmp3, 23); 
    }

};
