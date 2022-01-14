#pragma once

#include <Common.h>

#include "HAL.h"
#include <Comms.h>  

#include <Arduino.h>
#include <INA219.h>

namespace Actuators {
    
    extern uint32_t heaterCheckPeriod; // interval for checking valve current and voltages

    const float maxValveCurrent = 1.0;

    extern float act1Voltage;
    extern float act1Current;
    extern uint8_t act1State;
    extern Task *stop1;
    const uint8_t act1Pin1 = HAL::hBrg1Pin1;
    const uint8_t act1Pin2 = HAL::hBrg1Pin2;

    extern float act2Voltage;
    extern float act2Current;
    extern uint8_t act2State;
    extern Task *stop2;
    const uint8_t act2Pin1 = HAL::hBrg2Pin1;
    const uint8_t act2Pin2 = HAL::hBrg2Pin2;

    extern float act3Voltage;
    extern float act3Current;
    extern uint8_t act3State;
    extern Task *stop3;
    const uint8_t act3Pin1 = HAL::hBrg3Pin1;
    const uint8_t act3Pin2 = HAL::hBrg3Pin2;

    extern float act4Voltage;
    extern float act4Current;
    extern uint8_t act4State;
    extern Task *stop4;
    const uint8_t act4Pin1 = HAL::hBrg4Pin1;
    const uint8_t act4Pin2 = HAL::hBrg4Pin2;

    extern float act5Voltage;
    extern float act5Current;
    extern uint8_t act5State;
    extern Task *stop5;
    const uint8_t act5Pin1 = HAL::hBrg5Pin1;
    const uint8_t act5Pin2 = HAL::hBrg5Pin2;

    extern float act6Voltage;
    extern float act6Current;
    extern uint8_t act6State;
    extern Task *stop6;
    const uint8_t act6Pin1 = HAL::hBrg6Pin1;
    const uint8_t act6Pin2 = HAL::hBrg6Pin2;

    extern float act7Voltage;
    extern float act7Current;
    extern uint8_t act7State;
    extern Task *stop7;
    const uint8_t act7Pin1 = HAL::hBrg7Pin1;
    const uint8_t act7Pin2 = HAL::hBrg7Pin2;

    // TODO: Set correct OC limits
    const uint8_t OClimits[7] = {4,4,4,4,4,4,4}; //(amps): Act 1, Act 2, Act 3, Act 4, Act 5, Act 6, Act 7
    const float stopCurrent = .1; // Stopped actuator current threshold (amps) 
    
    void extendAct1();
    void retractAct1();
    uint32_t stopAct1();
    void brakeAct1();
    void act1PacketHandler(Comms::Packet tmp);

    void extendAct2();
    void retractAct2();
    uint32_t stopAct2();
    void brakeAct2();
    void act2PacketHandler(Comms::Packet tmp);

    void extendAct3();
    void retractAct3();
    uint32_t stopAct3();
    void brakeAct3();
    void act3PacketHandler(Comms::Packet tmp);

    void extendAct4();
    void retractAct4();
    uint32_t stopAct4();
    void brakeAct4();
    void act4PacketHandler(Comms::Packet tmp);

    void extendAct5();
    void retractAct5();
    uint32_t stopAct5();
    void brakeAct5();
    void act5PacketHandler(Comms::Packet tmp);

    void extendAct6();
    void retractAct6();
    uint32_t stopAct6();
    void brakeAct6();
    void act6PacketHandler(Comms::Packet tmp);

    void extendAct7();
    void retractAct7();
    uint32_t stopAct7();
    void brakeAct7();
    void act7PacketHandler(Comms::Packet tmp);

    void actPacketHandler(Comms::Packet tmp, void (*extend)(), void (*retract)(), Task *stopTask);

    void sampleActuator(Comms::Packet *packet, INA219 *ina, float *voltage, float *current, uint8_t *actState, uint8_t actuatorID);

    void initActuators();

    void sampleActuator(Comms::Packet *packet, INA219 *ina, float *voltage, float *current);
    uint32_t act1Sample();
    uint32_t act2Sample();
    uint32_t act3Sample();
    uint32_t act4Sample();
    uint32_t act5Sample();
    uint32_t act6Sample();
    uint32_t act7Sample();
};