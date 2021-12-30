#include <Common.h>
#include <Comms.h>
#include "Actuators.h"
#include "Power.h"
#include "HAL.h"

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

Task taskTable[] = {
    // actuators
    {Actuators::act1Sample, 0},
    {Actuators::act2Sample, 0},
    {Actuators::act3Sample, 0},
    {Actuators::act4Sample, 0},
    {Actuators::act5Sample, 0},
    {Actuators::act6Sample, 0},
    {Actuators::act7Sample, 0},

    {Actuators::stopAct1, 0},
    {Actuators::stopAct2, 0},
    {Actuators::stopAct3, 0},
    {Actuators::stopAct4, 0},
    {Actuators::stopAct5, 0},
    {Actuators::stopAct6, 0},
    {Actuators::stopAct7, 0},

    // power
    {Power::battSample, 0},
    {Power::supply12Sample, 0}
};

#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

int main() {
    // hardware setup
    Serial.begin(115200);
    #ifdef DEBUG_MODE
    while(!Serial) {} // wait for user to open serial port (debugging only)
    #endif
    Actuators::stop1 = &taskTable[7];
    Actuators::stop2 = &taskTable[8];
    Actuators::stop3 = &taskTable[9];
    Actuators::stop4 = &taskTable[10];
    Actuators::stop5 = &taskTable[11];
    Actuators::stop6 = &taskTable[12];
    Actuators::stop7 = &taskTable[13];

    HAL::initHAL();
    Comms::initComms();
    Actuators::initActuators();
    Power::initPower();

    while(1) {
        uint32_t ticks = micros(); // current time in microseconds
        for(uint32_t i = 0; i < TASK_COUNT; i++) { // for each task, execute if next time >= current time
        if (ticks >= taskTable[i].nexttime)
            taskTable[i].nexttime = ticks + taskTable[i].taskCall();
        }
        Comms::processWaitingPackets();
    }
    return 0;
}
