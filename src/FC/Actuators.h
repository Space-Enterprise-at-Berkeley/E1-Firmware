#pragma once

#include <Common.h>

#include "HAL.h"
#include <Comms.h>  

#include <Arduino.h>

namespace Actuators {

    struct Actuator {
        uint8_t actuatorID;
        uint8_t statusPacketID;

        float voltage;
        float current;
        uint32_t period;
        uint8_t state;

        uint8_t pin1;
        uint8_t pin2;

        Task *stop;

        MuxChannel *muxChannel;
    };

    extern Actuator pressFlowRBV; 

    // TODO: Set correct OC limits
    const uint8_t OClimit = 30; //(amps)
    const float stopCurrent = .01; // Stopped actuator current threshold (amps). 
    // Current value is for press flow RBV
    
    void extendPressFlowRBV();
    void retractPressFlowRBV();
    uint32_t stopPressFlowRBV();
    void brakePressFlowRBV();
    void pressFlowRBVPacketHandler(Comms::Packet tmp, uint8_t ip);

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

    void sampleActuator(Actuator *actuator);

    void initActuators(Task *pressFlowStopTask);

    uint32_t pressFlowRBVSample();
};