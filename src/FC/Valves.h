#pragma once

#include <Common.h>

#include <HAL.h>
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

    extern Valve armValve;
    extern Valve igniter;
    extern Valve loxMainValve;
    extern Valve fuelMainValve;
    
    void initValves();

    void openArmValve();
    void closeArmValve(uint8_t OCShutoff = 0);

    void activateIgniter();
    void deactivateIgniter(uint8_t OCShutoff = 0);

    void openLoxMainValve();
    void closeLoxMainValve(uint8_t OCShutoff = 0);

    void openFuelMainValve();
    void closeFuelMainValve(uint8_t OCShutoff = 0);

    void sampleValve(Valve *valve);
    uint32_t armValveSample();
    uint32_t igniterSample();
    uint32_t loxMainValveSample();
    uint32_t fuelMainValveSample();
};