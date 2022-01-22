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

#include <Common.h>
#include <Comms.h>

#include <Arduino.h>
#include "BMP388_DEV.h"

namespace Barometer {
    extern float altitude, pressure, temperature;
    extern BMP388_DEV bmp388;

    void init();
    // altitude in meters (m), pressure in hectoPascals/millibar (hPa), temperature in degrees celcius (°C)
    uint32_t sampleAltPressTemp();
}
