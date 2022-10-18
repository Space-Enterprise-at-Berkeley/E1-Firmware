/**
 * @file Barometer.h
 * @author Jewook Ryu (ryu_jewook@berkeley.edu)
 * @brief 
 * @version 0.1
 * @date 2022-01-22
 * 
 * @copyright Copyright (c) 2022
 * @source: https://github.com/MartinL1/BMP388_DEV
 */

#pragma once

#include <Comms.h>
#include "HAL.h"

namespace Barometer {
    extern uint32_t bmUpdatePeriod;

    extern float baroAltitude, baroPressure, baroTemperature;

    void init(Task *zeroAltitudeTask);
    // altitude in meters (m), pressure in hectoPascals/millibar (hPa), temperature in degrees Celsius (C)
    uint32_t sampleAltPressTemp();

    uint32_t zeroAltitude();

    uint32_t getUpdatePeriod();

    float getAltitude();
}