#pragma once

#include <Common.h>

#include "HAL.h"
#include <Comms.h>  

#include <Arduino.h>

namespace Actuators {

    struct Actuator {
        uint8_t statusPacketID;
        uint8_t currentStatus; 

        bool hasVoltage = true;
        bool hasCurrent = true;

        float voltage;
        float current;
        uint32_t period;
        uint8_t state;

        uint8_t pin1;
        uint8_t pin2;

        MuxChannel *muxChannel;
    };
    
    extern uint32_t heaterCheckPeriod; // interval for checking valve current and voltages

    const float maxValveCurrent = 1.0;

    extern float pressFlowRBVVoltage;
    extern float pressFlowRBVCurrent;
    extern uint8_t pressFlowRBVState;
    extern Task *stopPressFlowRBV;
    const uint8_t pressFlowRBVPin1 = HAL::hBridge2Pin1;
    const uint8_t pressFlowRBVPin2 = HAL::hBridge2Pin2;

    // extern float act1Voltage;
    // extern float act1Current;
    // extern uint8_t act1State;
    // extern Task *stop1;
    // const uint8_t act1Pin1 = HAL::hBrg1Pin1;
    // const uint8_t act1Pin2 = HAL::hBrg1Pin2;

    // extern float act2Voltage;
    // extern float act2Current;
    // extern uint8_t act2State;
    // extern Task *stop2;
    // const uint8_t act2Pin1 = HAL::hBrg2Pin1;
    // const uint8_t act2Pin2 = HAL::hBrg2Pin2;

    // TODO: Set correct OC limits
    const uint8_t OClimits[7] = {4,4,4,4,4,4,4}; //(amps): Act 1, Act 2, Act 3, Act 4, Act 5, Act 6, Act 7
    const float stopCurrent = .1; // Stopped actuator current threshold (amps) 
    
    void extendPressFlowRBV();
    void retractPressFlowRBV();
    uint32_t stopPressFlowRBV();
    void brakePressFlowRBV();
    void pressFlowRBVPacketHandler(Comms::Packet tmp);

    // void extendAct1();
    // void retractAct1();
    // uint32_t stopAct1();
    // void brakeAct1();
    // void act1PacketHandler(Comms::Packet tmp);

    // void extendAct2();
    // void retractAct2();
    // uint32_t stopAct2();
    // void brakeAct2();
    // void act2PacketHandler(Comms::Packet tmp);

    void actPacketHandler(Comms::Packet tmp, void (*extend)(), void (*retract)(), Task *stopTask);

    void sampleActuator(Comms::Packet *packet, INA219 *ina, float *voltage, float *current, uint8_t *actState, uint8_t actuatorID);

    void initActuators();

    void sampleActuator(Comms::Packet *packet, INA219 *ina, float *voltage, float *current);
    uint32_t pressFlowRBVSample();
};