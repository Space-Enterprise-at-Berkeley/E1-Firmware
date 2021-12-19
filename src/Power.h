#pragma once
#include <Arduino.h>
#include <INA219.h>
#include <HAL.h>

namespace Power {

    
    extern uint32_t powerUpdatePeriod;

    // buffers to store voltages, current, power for each sensor
    extern long last_checked[HAL::numSupplies];
    extern float voltages[HAL::numSupplies];
    extern float currents[HAL::numSupplies];
    extern float powers[HAL::numSupplies];
    extern float energyConsumed[HAL::numSupplies];
    

    void initPower();
    uint32_t powerSample();
}