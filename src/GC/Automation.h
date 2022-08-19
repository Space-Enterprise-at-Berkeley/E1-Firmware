#pragma once

#include "Valves.h"
#include "Ducers.h"
#include "Thermocouples.h"
#include "Util.h"

#include <Arduino.h>
#include <stdlib.h>

namespace Automation {
    const float igniterTriggerThreshold = 0.06;
    const float breakWireThreshold = 11;
    const float currentThreshold = 0.2;
    const float mainValveCurrentThreshold = 0.08;

    const float thermocoupleAbsoluteThreshold = 200;
    const float thermocoupleThreshold = 150;
    const float thermocoupleRateThreshold = 20;

    const float loadCellThreshold = 100.0; //TODO actual value 100
    const float loadCellSampleRate = 0.0125;

    const float autoVentUpperThreshold = 630; //TODO actual value 650 - 620
    const float autoVentLowerThreshold = 610;

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
    void beginFlow(Comms::Packet packet, uint8_t ip);
    uint32_t flow();
    void beginManualAbortFlow(Comms::Packet packet, uint8_t ip);
    void beginAbortFlow();
    uint32_t abortFlow();
    uint32_t checkIgniter();
    void readLoadCell(Comms::Packet packet, uint8_t ip);
    uint32_t checkForTCAbort();
    uint32_t checkForLCAbort();
    uint32_t autoventFuelGemValveTask();
    uint32_t autoventLoxGemValveTask();

    void handleAutoSettings(Comms::Packet recv, uint8_t id);
};

#define STATE_ACTIVATE_IGNITER 0
#define STATE_DEACTIVATE_DISABLE_IGNITER 18
#define STATE_OPEN_ARM_VALVE 1
#define STATE_OPEN_LOX_VALVE 2
#define STATE_OPEN_FUEL_VALVE 3
#define STATE_BEGIN_THRUST_CHECK 4
#define STATE_CLOSE_FUEL_VALVE 5
#define STATE_CLOSE_LOX_VALVE 6
#define STATE_CLOSE_ARM_VALVE 7
#define STATE_NOMINAL_END_FLOW 8
#define STATE_ABORT_END_FLOW 10
#define STATE_ABORT_ENGINE_TEMP 11
#define STATE_ABORT_ENGINE_TEMP_ROC 12
#define STATE_ABORT_THRUST 13
#define STATE_ABORT_BREAKWIRE_BROKEN 14
#define STATE_ABORT_BREAKWIRE_UNBROKEN 15
#define STATE_ABORT_ARM_VALVE_LOW_CURRENT 16
#define STATE_ABORT_ARM_OR_LOX_VALVE_LOW_CURRENT 17
#define STATE_ABORT_FUEL_VALVE_LOW_CURRENT 18
#define STATE_MANUAL_SAFE_ABORT 19
