#pragma once

#include <Common.h>

#include <ADS8167.h>
#include <INA219.h>
#include <INA226.h>
#include <MCP9600.h>

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

namespace HAL {
    const float battShuntR = 0.002;
    const float battCurrMax = 8.0;
    const float supplyShuntR = 0.01;
    const float supplyCurrMax = 4.0;
    extern INA226 supplyBatt;
    extern INA219 supply12v;

    const float chanShuntR = 0.033;
    const float chanCurrMax = 4.0;
    extern INA219 chan0;
    extern INA219 chan1;
    extern INA219 chan2;
    extern INA219 chan3;
    extern INA219 chan4;
    extern INA219 chan5;
    extern INA219 chan6;
    extern INA219 chan7;
    extern INA219 chan8;
    extern INA219 chan9;
    extern INA219 chan10;

    const uint8_t hBrg1Pin1 = 2;
    const uint8_t hBrg1Pin2 = 3;
    const uint8_t hBrg2Pin1 = 4;
    const uint8_t hBrg2Pin2 = 5;
    const uint8_t hBrg3Pin1 = 6;
    const uint8_t hBrg3Pin2 = 7;
    const uint8_t hBrg4Pin1 = 11;
    const uint8_t hBrg4Pin2 = 12;
    const uint8_t hBrg5Pin1 = 24;
    const uint8_t hBrg5Pin2 = 25;
    const uint8_t hBrg6Pin1 = 28;
    const uint8_t hBrg6Pin2 = 29;
    const uint8_t hBrg7Pin1 = 37;
    const uint8_t hBrg7Pin2 = 36;

    const uint8_t ctl12vChan1 = 14;
    const uint8_t ctl12vChan2 = 15;
    
    const uint8_t ctl24vChan1 = 22;
    const uint8_t ctl24vChan2 = 23;

    void initHAL();
    
};