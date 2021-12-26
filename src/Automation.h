#pragma once

#include <Task.h>
#include <Valves.h>

#include <Arduino.h>

namespace Automation {
    extern Task *flowTask; // perform burn

    extern float loxLead;
    extern float burnTime;

    void initAutomation();
    void beginFlow();
    uint32_t flow();
};
