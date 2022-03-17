#include <Common.h>
#include <Comms.h>
#include "HAL.h"
#include "Automation.h"
#include "Apogee.h"
#include "BlackBox.h"
#include "Barometer.h"
#include "IMU.h"
#include "GPS.h"
#include "BreakWire.h"
#include "Camera.h"

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

Task taskTable[] = {
    // Barometer
    {Barometer::sampleAltPressTemp, 0},
    {Barometer::zeroAltitude, 0},

    {IMU::imuSample, 0},

    //GPS
    {GPS::latLongSample, 0},
    // {GPS::auxSample, 0},

    //Break Wire
    {BreakWire::sampleBreakWires, 0},

    //Automation
    {Automation::sendFlightModePacket, 0},

    //Apogee
    {Apogee::apogeeDetectionTask, 0},
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
    Barometer::init(&taskTable[1]);
    IMU::initIMU();
    GPS::initGPS();
    BlackBox::init();
    BreakWire::init();
    Camera::init();
    Apogee::initApogee();
    Automation::init();

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
