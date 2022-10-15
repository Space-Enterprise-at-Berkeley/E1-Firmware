#include <Common.h>
#include <Comms.h>

#include "GPS.h"
#include "IMU.h"
#include "BlackBox.h"
// #include "Barometer.h"
// #include "Apogee.h"

#include <Arduino.h>



Task taskTable[] = {
    {GPS::latLongSample, 0},
    {IMU::sampleIMU, 0},
    // {Barometer::sampleBarometer, 0},
    // {Apogee::checkForApogee, 0, false}
    {BlackBox::sendMetadataPacket, 0},
};



#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

int main() {
    // hardware setup
    Serial.begin(115200);
    Serial.println("hola.. delaying 5s..");
    delay(5000);
    
    #ifdef ERASE_BB
    while (!Serial); {}
    Serial.printf("Init'ing black box\n");
    BlackBox::init(10);
    Serial.printf("Erasing black box... \n");
    BlackBox::resetAll();
    Serial.printf("done!\n");
    while(1);
    #endif

    #ifdef DUMP_BB
    while (!Serial) {}
    int maxDumpPages = 50; //set to -1 to dump all, otherwise between 0 and 65536
    Serial.printf("Init'ing black box\n");
    BlackBox::init(10);
    Serial.printf("starting dump:\n");
    BlackBox::dump(maxDumpPages);
    while(1);
    #endif

    Comms::initComms();
    IMU::init();
    GPS::init();
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