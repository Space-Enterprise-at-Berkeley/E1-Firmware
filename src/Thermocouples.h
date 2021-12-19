#pragma once

#include <Arduino.h>
#include <Adafruit_MCP9600.h>

namespace Thermocouples {
    extern uint32_t tcUpdatePeriod;
    extern Comms::Packet tcPacket;

    const int numTCs = 4;
    extern float tcTemps[numTCs];
    extern uint8_t tcAddrs[numTCs];
    extern Adafruit_MCP9600 TCs[numTCs];
    
    void initThermocouples(TwoWire *tcBus);
    uint32_t tcSample();
};
