#include <Common.h>
#include <Comms.h>
#include "HAL.h"

#include <Arduino.h>

const uint32_t CAP_UPDATE_PERIOD = 100 * 1000; // 10 hz

uint32_t readCaps() {
    float cap0 = HAL::capSensor.readCapacitance0();
    float cap1 = HAL::capSensor.readCapacitance1();

    DEBUG(cap1);
    DEBUG("\n");

    Comms::Packet tmp = {.id = 121};
    Comms::packetAddFloat(&tmp, cap0);
    Comms::packetAddFloat(&tmp, cap1);
    Comms::emitPacket(&tmp);

    return CAP_UPDATE_PERIOD;
}

Task taskTable[] = {
    // capacitors
    {readCaps, 0},
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

    while(1) {
        uint32_t ticks = micros(); // current time in microseconds
        for(uint32_t i = 0; i < TASK_COUNT; i++) { // for each task, execute if next time >= current time
        if (taskTable[i].enabled && ticks >= taskTable[i].nexttime)
            // DEBUG(i);
            // DEBUG("\n");
            taskTable[i].nexttime = ticks + taskTable[i].taskCall();
        }
        Comms::processWaitingPackets();
    }
    return 0;
}
