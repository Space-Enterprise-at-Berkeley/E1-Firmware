#pragma once

#include "HAL.h"

#include <Comms.h>
#include <BNO055.h>

#include <Arduino.h>

namespace IMU {
    extern uint32_t imuUpdatePeriod;

    extern float qW;
    extern float qX;
    extern float qY;
    extern float qZ;
    extern float accelX;
    extern float accelY;
    extern float accelZ;

    void initIMU();
    uint32_t imuSample();
};