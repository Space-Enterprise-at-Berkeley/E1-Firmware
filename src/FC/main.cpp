#include <Common.h>
#include <Comms.h>

#include "GPS.h"
#include "IMU.h"
#include "BlackBox.h"
// #include "Barometer.h"
// #include "Apogee.h"

#include <Arduino.h>



Task taskTable[] = {
    // {GPS::latLongSample, 0},
    {IMU::sampleIMU, 0},
    // {Barometer::sampleBarometer, 0},
    // {Apogee::checkForApogee, 0, false}
};



#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

int main() {
    // hardware setup
    Serial.begin(115200);
    Serial.println("hola");
    Comms::initComms();
    IMU::init();
    // GPS::init();
    BlackBox::init(10);
    
    #ifdef DEBUG_MODE
    while(!Serial) {} // wait for user to open serial port (debugging only)
    #endif

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