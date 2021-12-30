#pragma once

#include <Common.h>

#include <HAL.h>
#include <Comms.h>

#include <Arduino.h>
#include <INA219.h>

namespace Valves {
    
    extern uint32_t valveCheckPeriod; // interval for checking valve current and voltages

    // 8 solenoids  // l2, l5, lg, p2, p5, pg, h, h enable
    // uint8_t solenoidPins[numSolenoids] = {5,  3,  1,  0,  2,  4, 6, 39};
    // const uint8_t numSolenoidCommands = 10;    //       l2, l5, lg, p2, p5, pg,  h, arm, launch , h enable
    // uint8_t solenoidCommandIds[numSolenoidCommands] = {20, 21, 22, 23, 24, 25, 26,  27, 28     , 31};
    // uint8_t solenoidINAAddrs[numSolenoids] = {0x40, 0x42, 0x44, 0x45, 0x43, 0x41};
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

    void sampleValve(Comms::Packet *packet, INA219 *ina, float *voltage, float *current);
    uint32_t armValveSample();
    uint32_t igniterSample();
    uint32_t loxMainValveSample();
    uint32_t fuelMainValveSample();
};