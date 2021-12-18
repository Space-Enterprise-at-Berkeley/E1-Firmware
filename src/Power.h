#pragma once
#include <Arduino.h>
#include <INA219.h>
#include <HAL.h>

namespace Power {

    const int numSupplies = 3;
    extern uint32_t powerUpdatePeriod;

    // buffers to store voltages, current, power for each sensor
    extern long last_checked[numSupplies];
    extern float voltages[numSupplies];
    extern float currents[numSupplies];
    extern float powers[numSupplies];
    extern float _energyConsumed[numSupplies];

    void initPower();
    uint32_t powerSample();
}