#pragma once

#include <Scheduler.h>
#include <Arduino.h>
#include <GpioExpander.h>
#include <tempController.h>

namespace Heaters
{
    class Heater{
        // contains data and configurations for all the heaters
        uint8_t _numHeaters;
        std::string **_names;
        HeaterCommand **_heaterCommands;
        float _maxPTHeaterCurrent;

        public:
        float *_currents;
        float *_voltages;
        float *_overCurrents;

        Heater(uint8_t numHeaters, std::string **names, uint8_t *heaterCommandIds, int tempSetPoint, int algorithmChoice, GpioExpander *expander, uint8_t *channels, TwoWire *wire, uint8_t *addrs, float shuntR, float maxExpectedCurrent, float maxPTHeaterCurrent);
        void controlTemp(uint32_t exec_time, int target);
        void readCurrentDraw(uint32_t exec_time, int target);
        void readBusVoltage(uint32_t exec_time, int target);
        void checkOverCurrent(uint32_t exec_time, int target);
    };

    class HeaterTask: public Task{
        
        Heater *_heater;
        int _target; // which heater is being targeted by task
        void (Heater::*_func)(uint32_t, int);

        HeaterTask(Heater* heater, int target, void (Heater::*func)(uint32_t, int));
        void run(uint32_t exec_time);
    };

    // helper function to instantiate heater class
    Heater& initHeater(TwoWire *localwire);
}
