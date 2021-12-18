#pragma once

#include <ADS8167.h>
#include <INA219.h>
#include <Arduino.h>

namespace HAL {
    extern ADS8167 adc1;
    extern ADS8167 adc2;
    extern INA219 ina1;
    extern INA219 ina2;
    extern INA219 ina3;
    uint32_t initHAL();
};