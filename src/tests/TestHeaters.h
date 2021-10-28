#pragma once

#include <Heaters.h>


namespace TestHeater
{
    class testFunc{
        /* A self-contained class to check basic heater functionality */
        Heaters::Heater* htrClass;
        uint32_t lastPrint;
        uint32_t heatStart;
        public:
        testFunc();
        void loop();
    };

    // class testPowerTasks{
    //     /* A self-contained class to check PowerTasks schedule and execute */
    //     Power::Power* powerClass;
    //     uint32_t lastPrint;
    //     testPowerTasks();
    //     void loop();
    // };

}