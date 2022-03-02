#pragma once

#include <Common.h>

#include "HAL.h"
#include <Comms.h>

#include <Arduino.h>
#include <INA219.h>
#include <MuxChannel.h>

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

        MuxChannel *muxChannel;
    };

    struct Actuator {
        uint8_t statusPacketID;

        bool hasVoltage = true;
        bool hasCurrent = true;

        float voltage;
        float current;
        uint32_t period;

        MuxChannel *muxChannel;
    };

    extern Valve armValve;
    extern Valve igniter;
    extern Valve loxMainValve;
    extern Valve fuelMainValve;
    extern Valve breakWire;
    extern Valve loxTankBottomHtr;
    extern Valve loxTankMidHtr;
    extern Valve loxTankTopHtr;
    extern Valve igniterEnableRelay;

    
    void initActuators();

    void openArmValve();
    void closeArmValve(uint8_t OCShutoff = 0);

    void activateIgniter();
    void deactivateIgniter(uint8_t OCShutoff = 0);

    void openLoxMainValve();
    void closeLoxMainValve(uint8_t OCShutoff = 0);

    void openFuelMainValve();
    void closeFuelMainValve(uint8_t OCShutoff = 0);

    void activateLoxTankBottomHtr();
    void deavtivateLoxTankBottomHtr(uint8_t OCShutoff = 0);
    
    void activateLoxTankMidHtr();
    void deavtivateLoxTankMidHtr(uint8_t OCShutoff = 0);

    void activateLoxTankTopHtr();
    void deavtivateLoxTankTopHtr(uint8_t OCShutoff = 0);

    void enableIgniter();
    void disableIgniter(uint8_t OCShutoff = 0);

    void sampleValve(Valve *valve);
    uint32_t armValveSample();
    uint32_t igniterSample();
    uint32_t loxMainValveSample();
    uint32_t fuelMainValveSample();
    uint32_t breakWireSample();
    uint32_t loxTankBottomHtrSample();
    uint32_t loxTankMidHtrSample();
    uint32_t loxTankTopHtrSample();
    uint32_t igniterEnableRelaySample();

    uint32_t chute1Sample();
    uint32_t chute2Sample();
    uint32_t cam1Sample();
    uint32_t rfAmpSample();
    uint32_t cam2Sample();
    uint32_t radioSample();
    uint32_t hBridge1Sample();
    uint32_t hBridge2Sample();
    uint32_t hBridge3Sample();
    uint32_t break2Sample();

};