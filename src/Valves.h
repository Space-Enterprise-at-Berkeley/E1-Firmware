#pragma once
#include <Arduino.h>
#include <INA219.h>
#include <HAL.h>

namespace Valves {
    
    extern uint32_t valveCheckPeriod; // interval for checking valve current and voltages

    // 8 solenoids  // l2, l5, lg, p2, p5, pg, h, h enable
    // uint8_t solenoidPins[numSolenoids] = {5,  3,  1,  0,  2,  4, 6, 39};
    // const uint8_t numSolenoidCommands = 10;    //       l2, l5, lg, p2, p5, pg,  h, arm, launch , h enable
    // uint8_t solenoidCommandIds[numSolenoidCommands] = {20, 21, 22, 23, 24, 25, 26,  27, 28     , 31};
    // uint8_t solenoidINAAddrs[numSolenoids] = {0x40, 0x42, 0x44, 0x45, 0x43, 0x41};
    const float maxValveCurrent = 1.0;

    extern bool valve_is_open[HAL::numValves];
    extern long last_checked[HAL::numValves];
    extern float currents[HAL::numValves];
    extern float voltages[HAL::numValves];
    
    void initValves();

    uint32_t check_currents(); // reads all currents, checks for overcurrent
    // uint32_t check_voltages(); // reads all voltages
 
    // helper functions
    void open_valve(int valve_num);
    void close_valve(int valve_num);

}