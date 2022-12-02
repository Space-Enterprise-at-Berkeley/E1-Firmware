#pragma once

#include <Common.h>

#include "HAL.h"
#include <Comms.h>

#include <Arduino.h>
#include <INA219.h>

namespace Valves {

    struct Valve {
        uint8_t valveID;
        uint8_t statePacketID;
        uint8_t statusPacketID;

        uint8_t pin; 
        float voltage;
        float current;
        float ocThreshold;
        uint32_t period;

        INA219 *ina;
    };

    extern Valve loxDomeHeater;
    extern Valve fuelDomeHeater;

    void initValves();

    void openLoxDomeHeater();
    void closeLoxDomeHeater(uint8_t OCShutoff = 0);

    void openFuelDomeHeater();
    void closeFuelDomeHeater(uint8_t OCShutoff = 0);

    void sampleValve(Valve *valve);
    uint32_t loxDomeHeaterSample();
    uint32_t fuelDomeHeaterSample();
};