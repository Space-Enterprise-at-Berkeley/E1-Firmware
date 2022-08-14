#include "Automation.h"
#include <Common.h>
#include <Comms.h>
#include "Ducers.h"
#include "Power.h"
#include "Actuators.h"
#include "HAL.h"
// #include "Thermocouples.h"
#include "OCHandler.h"

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>


Task taskTable[] = {
    //automation
    {Automation::flow, 0, false},//0
    {Automation::abortFlow, 0, false},//1
    {Automation::checkIgniter, 0},//2
    {Automation::checkForTCAbort, 0, false},//3
    {Automation::checkForLCAbort, 0, false},//4
    {Automation::autoventFuelGemValveTask, 0},//5
    {Automation::autoventLoxGemValveTask, 0},//6

    // ducers
    {Ducers::ptSample, 0},//7
    {Ducers::pressurantPTROCSample, 0}, //8 

    // power
    {Power::supply8Sample, 0},//9

    // thermocouples
    //{Thermocouples::tcSample, 0},
    // {Thermocouples::tc0Sample, 0},
    // {Thermocouples::tc1Sample, 0},
    // {Thermocouples::tc2Sample, 0},
    // {Thermocouples::tc3Sample, 0},

    // valves
    {Actuators::armValveSample, 0},//10
    {Actuators::igniterSample, 0},//11
    {Actuators::loxMainValveSample, 0},//12
    {Actuators::fuelMainValveSample, 0},//13
    {Actuators::loxGemValveSample, 0},//14
    {Actuators::fuelGemValveSample, 0},//15
    {Actuators::breakWireSample, 0},//16
    {Actuators::toggleLoxGemValveTask, 0},//17
    {Actuators::toggleFuelGemValveTask, 0},//18
    {Actuators::igniterEnableRelaySample, 0, false},//19 TODO: Assign channel

    // heaters
    {Actuators::RQDSample, 0, false},//20 TODO: Assign channel
    {Actuators::mainValveVentSample, false, 0},//21 TODO: Assign channel

    // current sense mux
    {Actuators::chute1Sample, 0},//22
    {Actuators::chute2Sample, 0},//23
    {Actuators::cam1Sample, 0},//24
    {Actuators::rfAmpSample, 0},//25
    {Actuators::cam2Sample, 0},//26
    {Actuators::radioSample, 0},//27
    {Actuators::hBridge1Sample, 0},//28
    {Actuators::hBridge2Sample, 0},//29
    {Actuators::hBridge3Sample, 0},//30
    {Actuators::break2Sample, 0},//31
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
    Actuators::initActuators(&taskTable[23], &taskTable[24]);
    Thermocouples::initThermocouples();
    OCHandler::initOCHandler(20); // TODO: this seems to be causing code to crash... 

    while(1) {
        for(uint32_t i = 0; i < TASK_COUNT; i++) { // for each task, execute if next time >= current time
            uint32_t ticks = micros(); // current time in microseconds
            if (taskTable[i].nexttime - ticks > UINT32_MAX / 2 && taskTable[i].enabled) {
                taskTable[i].nexttime = ticks + taskTable[i].taskCall();
                Serial.print("Task done successfully ");
                Serial.println(i);
            }
        }
        Comms::processWaitingPackets();
    }
    return 0;
}
