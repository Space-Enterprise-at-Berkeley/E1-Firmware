#pragma once

#include "Valves.h"

#include <Arduino.h>
#include <stdlib.h>

namespace Automation {
    const float igniterTriggerThreshold = 0.06;
    const float breakWireThreshold = 23;
    const float currentThreshold = 0.2;

    extern Task *flowTask; // perform burn
    extern Task *abortFlowTask;

    extern float loxLead;
    extern float burnTime;

    extern bool igniterEnabled;
    extern bool breakwireEnabled;
    extern bool igniterTriggered;

    void initAutomation();
    void beginFlow(Comms::Packet packet);
    uint32_t flow();
    void beginAbortFlow(Comms::Packet packet);
    void beginAbortFlow();
    uint32_t abortFlow();
    uint32_t checkIgniter();
};
