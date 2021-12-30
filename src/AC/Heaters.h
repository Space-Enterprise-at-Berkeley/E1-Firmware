#pragma once

#include <Common.h>

#include "HAL.h"
#include <Comms.h>

#include <Arduino.h>
#include <INA219.h>

namespace Heaters {
    
    extern uint32_t heaterCheckPeriod; // interval for checking valve current and voltages

    const float maxValveCurrent = 1.0;

    extern float ctl12vChan1Voltage;
    extern float arctl12vChan1Current;
    const uint8_t ctl12vChan1Pin = HAL::ctl12vChan1;

    extern float ctl12vChan2Voltage;
    extern float ctl12vChan2Current;
    const uint8_t ctl12vChan2Pin = HAL::ctl12vChan2;

    extern float ctl24vChan1Voltage;
    extern float ctl24vChan1Current;
    const uint8_t ctl24vChan1Pin = HAL::ctl24vChan1;

    extern float ctl24vChan2Voltage;
    extern float ctl24vChan2Current;
    const uint8_t ctl24vChan2Pin = HAL::ctl24vChan2;
    
    void initHeaters();

    void sampleHeater(Comms::Packet *packet, INA219 *ina, float *voltage, float *current);
    uint32_t ctl12vChan1Sample();
    uint32_t ctl12vChan2Sample();
    uint32_t ctl24vChan1Sample();
    uint32_t ctl24vChan2Sample();
};