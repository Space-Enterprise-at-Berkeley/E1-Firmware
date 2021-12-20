#pragma once

#include <HAL.h>
#include <Comms.h>

#include <Arduino.h>
#include <INA219.h>

namespace Power {

    
    extern uint32_t powerUpdatePeriod;

    // buffers to store voltages, current, power for each sensor
    extern float battVoltage;
    extern float battCurrent;
    extern float battPower;

    extern float supply12Voltage;
    extern float supply12Current;
    extern float supply12Power;

    extern float supply8Voltage;
    extern float supply8Current;
    extern float supply8Power;
    
    void initPower();
    uint32_t battSample();
    uint32_t supply12Sample();
    uint32_t supply8Sample();
}