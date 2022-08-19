#pragma once

#include <Common.h>

#include "HAL.h"
#include <Comms.h>

#include <Arduino.h>
#include <MuxChannel.h>

namespace Valves {

    struct Valve {
        uint8_t valveID;
        uint8_t statePacketID;
        uint8_t statusPacketID;

        uint8_t pin; // use either pin or expanderPin, and set the other to 255
        uint8_t expanderPin;
        float voltage;
        float current;
        float ocThreshold;
        uint32_t period;

        MuxChannel *muxChannel;
    };

    void initValves(Task *toggleLoxGemValveTask, Task *toggleFuelGemValveTask);

    void openFuelGemValve();
    void closeFuelGemValve(uint8_t OCShutoff = 0);

    void openLoxGemValve(); 
    void closeLoxGemValve(uint8_t OCShutoff = 0);

    void sampleValve(Valve *valve);
    uint32_t loxGemValveSample();
    uint32_t fuelGemValveSample();
    uint32_t toggleFuelGemValveTask();
    uint32_t toggleLoxGemValveTask();
};