#pragma once

#include <Scheduler.h>
#include <INA219.h>
#include <Arduino.h>
#include <common_fw.h>

namespace Power
{   
    // 3 power supply monitors: 12V, 8V, Battery
    // uses INA219 - current/voltage/power monitoring
    class Power{
        
        INA219 ** _supplyMonitors;
        const uint8_t * _addrs;
        TwoWire * _localWire;
        float * _energyConsumed;

        public:
        uint8_t numSupplies;
        long * last_checked;
        float * voltages;
        float * currents;
        float * powers;

        Power(uint8_t numPowerSupplies, INA219 ** supplyMons, uint8_t * addrs, float rShunt, float maxExpectedCurrent, TwoWire *localWire);

        void readVoltage(uint32_t exec_time, int target);

        void readCurrent(uint32_t exec_time, int target);

        void readPowerConsumption(uint32_t exec_time, int target);

        void readAllBatteryStats(uint32_t exec_time, int target);

    };

    class PowerTask: public Task {
        Power *_power;
        int _target; // which sensor is being targeted by task
        void (Power::*_func)(uint32_t, int);

        public:
        PowerTask(Power* power, int target, void (Power::*func)(uint32_t, int));
        void run(uint32_t exec_time);
    };

    // helper function to instantiate power supply monitors
    Power* initPower(TwoWire *localwire);
}
