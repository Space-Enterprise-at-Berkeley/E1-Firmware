#include "Automation.h"
#include <Common.h>
#include <Comms.h>
#include "Power.h"
#include "Valves.h"
#include "HAL.h"
#include "Thermocouples.h"

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

Task taskTable[] = {
    //automation
    {Automation::flow, 0, false},
    {Automation::abortFlow, 0, false},
    {Automation::checkIgniter, 0},

    // power
    {Power::battSample, 0},
    {Power::supply12Sample, 0},
    {Power::supply8Sample, 0},

    // thermocouples
    // {Thermocouples::tcSample, 0},
    {Thermocouples::tc0Sample, 0},
    {Thermocouples::tc1Sample, 0},
    {Thermocouples::tc2Sample, 0},
    {Thermocouples::tc3Sample, 0},

    // valves
    {Valves::armValveSample, 0},
    {Valves::igniterSample, 0},
    {Valves::loxMainValveSample, 0},
    {Valves::fuelMainValveSample, 0},
    {Valves::breakWireSample, 0},
    {Valves::igniterEnableRelaySample, 0},
    {Valves::RQDSample, 0},
    {Valves::mainValveVentSample, 0},
};

#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

int main() {
    // hardware setup
    Serial.begin(115200);
    #ifdef DEBUG_MODE
    while(!Serial) {} // wait for user to open serial port (debugging only)
    #endif
    Automation::initAutomation(&taskTable[0], &taskTable[1]);
    HAL::initHAL();
    Comms::initComms();
    Power::initPower();
    Valves::initValves();
    Thermocouples::initThermocouples();

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
