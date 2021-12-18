#pragma once

#include <ADS8167.h>

#include <Arduino.h>

namespace HAL {
    extern ADS8167 adc1;
    extern ADS8167 adc2;

    uint32_t initHAL();
};