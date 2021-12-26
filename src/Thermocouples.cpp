#pragma once

#include <Arduino.h>
#include <Comms.h>
#include <Thermocouples.h>

namespace Thermocouples {
    uint32_t tcUpdatePeriod = 100 * 1000;
    Comms::Packet tcPacket = {.id = 9};

    float engineTC1Value;
    float engineTC2Value;
    float engineTC3Value;
    float engineTC4Value;

    void initThermocouples() {
    }

    uint32_t tcSample() {
        // read from all TCs in sequence
        engineTC1Value = HAL::tcAmp0.readThermocouple();
        engineTC2Value = HAL::tcAmp1.readThermocouple();
        engineTC3Value = HAL::tcAmp2.readThermocouple();
        engineTC4Value = HAL::tcAmp3.readThermocouple();
        
        tcPacket.len = 0;
        Comms::packetAddFloat(&tcPacket, engineTC1Value);
        Comms::packetAddFloat(&tcPacket, engineTC2Value);
        Comms::packetAddFloat(&tcPacket, engineTC3Value);
        Comms::packetAddFloat(&tcPacket, engineTC4Value);
        
        Comms::emitPacket(&tcPacket);
        // return the next execution time
        return tcUpdatePeriod;
    }
};
