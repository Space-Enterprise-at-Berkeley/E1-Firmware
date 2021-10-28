#pragma once

#include <Scheduler.h>
#include <Power.h>

namespace TestPower
{
    class testReadouts{
        /* A self-contained class to check basic power (5V, 9V) readouts work */
        Power::Power* powerClass;
        public:
        testReadouts();
        void loop();
    };

    class testPowerTasks{
        /* A self-contained class to check PowerTasks schedule and execute */
        Power::Power* powerClass;
        uint32_t lastPrint;
        public:
        testPowerTasks();
        void loop();
    };

}