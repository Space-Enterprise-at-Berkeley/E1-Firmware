#pragma once

#include "Valves.h"
#include "Thermocouples.h"

#include <Arduino.h>
#include <stdlib.h>

namespace Automation {
    const float igniterTriggerThreshold = 0.06;
    const float breakWireThreshold = 23;
    const float currentThreshold = 0.2;

    const float thermocoupleAbsoluteThreshold = 200;
    const float thermocoupleThreshold = 150;
    const float thermocoupleRateThreshold = 20;

    const float loadCellThreshold = 100.0; //TODO actual value 100
    const float loadCellSampleRate = 0.0125;

    extern Task *flowTask; // perform burn
    extern Task *abortFlowTask;
    extern Task *checkForTCAbortTask;
    extern Task *checkForLCAbortTask;

    extern uint32_t loxLead;
    extern uint32_t burnTime;

    extern bool igniterEnabled;
    extern bool breakwireEnabled;
    extern bool igniterTriggered;

    void initAutomation(Task *flowTask, Task *abortFlowTask, Task *checkForTCAbortTask, Task *checkForLCAbortTask);
    void beginFlow(Comms::Packet packet);
    uint32_t flow();
    void beginAbortFlow(Comms::Packet packet);
    void beginAbortFlow();
    uint32_t abortFlow();
    uint32_t checkIgniter();
    void readLoadCell(Comms::Packet packet);
    uint32_t checkForTCAbort();
    uint32_t checkForLCAbort();

    void handleAutoSettings(Comms::Packet recv);
};
