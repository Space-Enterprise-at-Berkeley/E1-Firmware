#pragma once

#include <Valves.h>
#include <Thermocouples.h>

#include <Arduino.h>
#include <stdlib.h>

namespace Automation {
    const float igniterTriggerThreshold = 0.06;
    const float breakWireThreshold = 23;
    const float currentThreshold = 0.2;
    const float thermocoupleThreshold = 70;

    extern Task *flowTask; // perform burn
    extern Task *abortFlowTask;
    extern Task *checkForAbortTask;

    extern float loxLead;
    extern float burnTime;

    extern bool igniterTriggered;
    bool thermocoupleThresholdReached;

    void initAutomation();
    void beginFlow(Comms::Packet packet);
    uint32_t flow();
    void beginAbortFlow(Comms::Packet packet);
    uint32_t abortFlow();
    uint32_t checkIgniter();
    uint32_t checkForAbort();
};
