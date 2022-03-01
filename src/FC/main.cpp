#include "Automation.h"
#include <Common.h>
#include <Comms.h>
#include "Ducers.h"
#include "Power.h"
#include "Actuators.h"
#include "HAL.h"
#include "Thermocouples.h"
#include "OCHandler.h"

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

Task taskTable[] = {
    //automation
    {Automation::flow, 0, false},
    {Automation::abortFlow, 0, false},
    {Automation::checkIgniter, 0},
    {Automation::checkForTCAbort, 0, false},
    {Automation::checkForLCAbort, 0, false},

    // ducers
    {Ducers::ptSample, 0},
    {Ducers::pressurantPTROCSample, 0},

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
    {Actuators::armValveSample, 0},
    {Actuators::igniterSample, 0},
    {Actuators::loxMainValveSample, 0},
    {Actuators::fuelMainValveSample, 0},
    {Actuators::breakWireSample, 0},
    {Actuators::igniterEnableRelaySample, 0},

    // heaters
    {Actuators::loxTankBottomHtrSample, 0},
    {Actuators::loxTankMidHtrSample, 0},
    {Actuators::loxTankTopHtrSample, 0},

    // current sense mux
    {Actuators::chute1Sample, 0},
    {Actuators::chute2Sample, 0},
    {Actuators::cam1Sample, 0},
    {Actuators::rfAmpSample, 0},
    {Actuators::cam2Sample, 0},
    {Actuators::radioSample, 0},
    {Actuators::valve1Sample, 0},
    {Actuators::valve2Sample, 0},
    {Actuators::valve3Sample, 0},
    {Actuators::valve4Sample, 0},
    {Actuators::valve5Sample, 0},
    {Actuators::valve6Sample, 0},
    {Actuators::hBridge1Sample, 0},
    {Actuators::hBridge2Sample, 0},
    {Actuators::hBridge3Sample, 0},
    {Actuators::break1Sample, 0},
    {Actuators::break2Sample, 0},
};

#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

int main() {
    // hardware setup
    Serial.begin(115200);
    #ifdef DEBUG_MODE
    while(!Serial) {} // wait for user to open serial port (debugging only)
    #endif
    Automation::initAutomation(&taskTable[0], &taskTable[1], &taskTable[3], &taskTable[4]);
    HAL::initHAL();
    Comms::initComms();
    Ducers::initDucers();
    Power::initPower();
    Actuators::initActuators();
    Thermocouples::initThermocouples();
    OCHandler::initOCHandler();

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
