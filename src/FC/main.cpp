#include <Common.h>
#include <Comms.h>
#include "Ducers.h"
#include "Power.h"
#include "Actuators.h"
#include "Valves.h"
#include "HAL.h"
#include "Thermocouples.h"
#include "OCHandler.h"

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>


Task taskTable[] = {
    {Actuators::stopPressFlowRBV, 0, false},

    // ducers
    {Ducers::ptSample, 0},
    {Ducers::pressurantPTROCSample, 0},

    // power
    {Power::supply8Sample, 0},

    // thermocouples
    {Thermocouples::tc0Sample, 0},
    {Thermocouples::tc1Sample, 0},
    {Thermocouples::tc2Sample, 0},
    // {Thermocouples::tc3Sample, 0},

    // valves
    {Valves::loxGemValveSample, 0},
    {Valves::fuelGemValveSample, 0},
    {Valves::toggleLoxGemValveTask, 0},
    {Valves::toggleFuelGemValveTask, 0},

    // actuator
    {Actuators::pressFlowRBVSample, 0},
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
    Actuators::initActuators();
    Thermocouples::initThermocouples();
    OCHandler::initOCHandler(20); 

    while(1) {
        for(uint32_t i = 0; i < TASK_COUNT; i++) { // for each task, execute if next time >= current time
            uint32_t ticks = micros(); // current time in microseconds
            if (taskTable[i].nexttime - ticks > UINT32_MAX / 2 && taskTable[i].enabled) {
                taskTable[i].nexttime = ticks + taskTable[i].taskCall();
            }
        }
        Comms::processWaitingPackets();
    }
    return 0;
}
