#pragma once

#include <ADS8167.h>
#include <INA219.h>
#include <Arduino.h>
#include <SPI.h>

namespace HAL {
    extern ADS8167 adc1;
    extern ADS8167 adc2;

    const int numSupplies = 3;
    extern uint8_t powerAddrs[numSupplies];
    extern INA219* supplyMonitors[numSupplies];

    const int numValves = 8;
    extern uint8_t valveAddrs[numValves];
    extern uint8_t valvePins[numValves];
    extern INA219* valveMonitors[numValves];

    void initHAL();
    
};