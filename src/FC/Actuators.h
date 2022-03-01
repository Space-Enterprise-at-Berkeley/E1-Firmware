#pragma once

#include <Common.h>

#include "HAL.h"
#include <Comms.h>

#include <Arduino.h>
#include <INA219.h>

namespace Actuators {

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

    struct MuxActuator {
        uint8_t statusPacketID;

        uint8_t channel;
        float voltage;
        float current;
        bool continuity;
        uint32_t period;
    };

    //pins on the teensy
    const uint8_t muxSelect1Pin = 3;
    const uint8_t muxSelect2Pin = 4;
    const uint8_t muxSelect3Pin = 5;
    const uint8_t muxSelect4Pin = 6;
    const uint8_t muxCurrentPin = 40;
    const uint8_t muxContinuityPin = 16;

    extern MuxActuator chute1;
    extern MuxActuator chute2;
    extern MuxActuator cam1;
    extern MuxActuator rfAmp;
    extern MuxActuator cam2;
    extern MuxActuator radio;
    extern MuxActuator valve1;
    extern MuxActuator valve2;
    extern MuxActuator valve3;
    extern MuxActuator valve4;
    extern MuxActuator valve5;
    extern MuxActuator valve6;
    extern MuxActuator hBridge1;
    extern MuxActuator hBridge2;
    extern MuxActuator hBridge3;
    extern MuxActuator break1;
    extern MuxActuator mux3;

    
    void initActuators(Task *toggleLoxGemValveTask, Task *toggleFuelGemValveTask);

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

    void sampleMuxActuator(MuxActuator *muxActuator);
    uint32_t chute1Sample();
    uint32_t chute2Sample();
    uint32_t cam1Sample();
    uint32_t rfAmpSample();
    uint32_t cam2Sample();
    uint32_t radioSample();
    uint32_t valve1Sample();
    uint32_t valve2Sample();
    uint32_t valve3Sample();
    uint32_t valve4Sample();
    uint32_t valve5Sample();
    uint32_t valve6Sample();
    uint32_t hBridge1Sample();
    uint32_t hBridge2Sample();
    uint32_t hBridge3Sample();
    uint32_t break1Sample();
    uint32_t break2Sample();
};
