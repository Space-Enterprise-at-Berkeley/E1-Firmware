#pragma once

#include <Common.h>

#include <FDC2214.h>

#include <Arduino.h>
#include <Wire.h>

namespace HAL {
    extern FDC2214 capSensor;

    void initHAL();
    
};