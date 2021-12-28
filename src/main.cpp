#include <Common.h>
#include <Comms.h>
#include <Ducers.h>
#include <Power.h>
#include <Valves.h>
#include <HAL.h>

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

Task taskTable[] = {
    // ducers
    {Ducers::ptSample, 0},

    // power
    {Power::battSample, 0},
    {Power::supply12Sample, 0},
    {Power::supply8Sample, 0},

    // valves
    {Valves::armValveSample, 0},
    {Valves::igniterSample, 0},
    {Valves::loxMainValveSample, 0},
    {Valves::fuelMainValveSample, 0},
};

#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

int main() {
    // hardware setup
    Serial.begin(115200);
    #ifdef DEBUG_MODE
    while(!Serial) {} // wait for user to open serial port (debugging only)
    #endif
    HAL::initHAL();
    Comms::initComms();
    Ducers::initDucers();
    Power::initPower();
    Valves::initValves();

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
