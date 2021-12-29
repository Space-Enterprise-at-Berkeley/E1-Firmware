#pragma once

#include <Common.h>

#include <HAL.h>
#include <Comms.h>

#include <Arduino.h>
#include <INA219.h>

namespace Valves {
    extern uint32_t valveCheckPeriod; // interval for checking valve current and voltages

    const float maxValveCurrent = 1.0;

    extern float armVoltage;
    extern float armCurrent;
    const uint8_t armPin = HAL::chan0Pin;

    extern float igniterVoltage;
    extern float igniterCurrent;
    const uint8_t igniterPin = HAL::chan1Pin;

    extern float loxMainVoltage;
    extern float loxMainCurrent;
    const uint8_t loxMainPin = HAL::chan2Pin;

    extern float fuelMainVoltage;
    extern float fuelMainCurrent;
    const uint8_t fuelMainPin = HAL::chan3Pin;
    
    void initValves();

    void openArmValve();
    void closeArmValve();

    void activateIgniter();
    void deactivateIgniter();

    void openLoxMainValve();
    void closeLoxMainValve();

    void openFuelMainValve();
    void closeFuelMainValve();

    void sampleValve(Comms::Packet *packet, INA219 *ina, float *voltage, float *current);
    uint32_t armValveSample();
    uint32_t igniterSample();
    uint32_t loxMainValveSample();
    uint32_t fuelMainValveSample();
};