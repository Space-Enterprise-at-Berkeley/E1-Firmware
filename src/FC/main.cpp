#include <Common.h>
#include <Comms.h>

#include "GPS.h"
#include "IMU.h"
#include "Barometer.h"
#include "Apogee.h"

#include <Arduino.h>


// Task taskTable[] = {
//     {IMU::sampleIMU, 0},
//     {Barometer::sampleBarometer, 0},
//     {Apogee::checkForApogee, 0, false}
// };

// #define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

int main() {
    // hardware setup
    Serial.begin(115200);
    
    #ifdef DEBUG_MODE
    while(!Serial) {} // wait for user to open serial port (debugging only)
    #endif
    setup();
    while(1) {
        loop();
    }
    return 0;
}