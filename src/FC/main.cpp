#include <Common.h>
#include <Comms.h>
#include "HAL.h"

#include "Barometer.h"
#include "IMU.h"
#include "GPS.h"

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

Task taskTable[] = {
    // Barometer
    {Barometer::sampleAltPressTemp, 0},

    {IMU::imuSample, 0},

    //GPS
    {GPS::latLongSample, 0},
    // {GPS::auxSample, 0},
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
    Barometer::init();
    IMU::initIMU();
    GPS::initGPS();

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
