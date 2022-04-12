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

        uint8_t pin; // use either pin or expanderPin, and set the other to 255
        uint8_t expanderPin;
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
    extern Valve breakWire;
    extern Valve RQD;
    extern Valve mainValveVent;
    extern Valve igniterEnableRelay;

    void initValves(Task *toggleLoxGemValveTask, Task *toggleFuelGemValveTask);

    void openArmValve();
    void closeArmValve(uint8_t OCShutoff = 0);

    void activateIgniter();
    void deactivateIgniter(uint8_t OCShutoff = 0);

    void openLoxMainValve();
    void closeLoxMainValve(uint8_t OCShutoff = 0);

    void openFuelMainValve();
    void closeFuelMainValve(uint8_t OCShutoff = 0);

    void openFuelGemValve();
    void closeFuelGemValve(uint8_t OCShutoff = 0);

    void openLoxGemValve();
    void closeLoxGemValve(uint8_t OCShutoff = 0);

    void activateRQD();
    void deactivateRQD(uint8_t OCShutoff = 0);
    
    void activateMainValveVent();
    void deactivateMainValveVent(uint8_t OCShutoff = 0);

    void enableIgniter();
    void disableIgniter(uint8_t OCShutoff = 0);

    void sampleValve(Valve *valve);
    uint32_t armValveSample();
    uint32_t igniterSample();
    uint32_t loxMainValveSample();
    uint32_t fuelMainValveSample();
    uint32_t loxGemValveSample();
    uint32_t fuelGemValveSample();
    uint32_t breakWireSample();
    uint32_t RQDSample();
    uint32_t mainValveVentSample();
    uint32_t toggleFuelGemValveTask();
    uint32_t toggleLoxGemValveTask();
    uint32_t igniterEnableRelaySample();
};