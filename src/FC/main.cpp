#include <Common.h>
#include <Comms.h>
#include "Ducers.h"
#include "Power.h"
#include "Actuators.h"
// #include "CapFill.h"
#include "Valves.h"
#include "HAL.h"
#include "Thermocouples.h"
#include "OCHandler.h"

// #include "BlackBox.h"
#include "Barometer.h"
#include "IMU.h"
#include "GPS.h"

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#define RS485_SERIAL Serial8

Task taskTable[] = {
    {Actuators::stopPressFlowRBV, 0, false},
    {Valves::toggleLoxGemValveTask, 0, false},
    {Valves::toggleFuelGemValveTask, 0, false},

    {Valves::autoventLoxGemValveTask, 0},
    {Valves::autoventFuelGemValveTask, 0},

    // ducers
    {Ducers::ptSample, 0},

    // power
    {Power::supply8Sample, 0},

    // thermocouples
    // {Thermocouples::tc0Sample, 0},
    // {Thermocouples::tc1Sample, 0},
    // {Thermocouples::tc2Sample, 0},
    // {Thermocouples::tc3Sample, 0},

    // valves
    {Valves::loxGemValveSample, 0},
    {Valves::fuelGemValveSample, 0},

    // actuator
    {Actuators::pressFlowRBVSample, 0},

    {IMU::imuSample, 0},

    //GPS
    {GPS::latLongSample, 0},
    // {GPS::auxSample, 0},

    // Barometer
    {Barometer::sampleAltPressTemp, 0},
    // {Barometer::zeroAltitude, 0},

    // Cap fill
    // {CapFill::sampleCapFill, 0}
};

#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

int main() {
    // hardware setup
    Serial.begin(115200);
    // RS-485 RX/TX is Serial8 (pins 34, 35)
    // RS485_SERIAL.begin(115200); // Serial for capfill
    #ifdef DEBUG_MODE
    while(!Serial) {} // wait for user to open serial port (debugging only)
    #endif
    HAL::initHAL();
    Comms::initComms();
    Ducers::initDucers();
    Power::initPower();
    Actuators::initActuators(&taskTable[0]);
    Valves::initValves(&taskTable[1], &taskTable[2]);
    // Thermocouples::initThermocouples();
    OCHandler::initOCHandler(20);
    GPS::initGPS();
    // CapFill::initCapFill();

    while(1) {
        for(uint32_t i = 0; i < TASK_COUNT; i++) { // for each task, execute if next time >= current time
            uint32_t ticks = micros(); // current time in microseconds
            if (taskTable[i].nexttime - ticks > UINT32_MAX / 2 && taskTable[i].enabled) {
                DEBUG("Task ID: ");
                DEBUG(i);
                DEBUG("\n");
                DEBUG_FLUSH();
                taskTable[i].nexttime = ticks + taskTable[i].taskCall();
            }
        }
        Comms::processWaitingPackets();
    }
    return 0;
}
