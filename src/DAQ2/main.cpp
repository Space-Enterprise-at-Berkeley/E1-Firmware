#include <Common.h>
#include <Comms.h>
#include "HAL.h"
#include "LoadCells.h"

#include <Arduino.h>

const uint32_t TC_UPDATE_PERIOD = 100 * 1000; // 10 hz

uint32_t tc1Sample() {
    float sensorValue = analogRead(HAL::tcAmp1_pin);
    float temp = (3.3*(sensorValue/1024)-1.25)/0.005;
    DEBUG("TC 1: ");
    DEBUG(temp);
    DEBUG("\n");
    Serial.flush();

    // Comms::Packet p = {.id = 110};
    // Comms::packetAddFloat(&p, temp);
    // Comms::emitPacket(&p);

    return TC_UPDATE_PERIOD;
}

uint32_t tc2Sample() {
    float sensorValue = analogRead(HAL::tcAmp2_pin);
    float temp = (3.3*(sensorValue/1024)-1.25)/0.005;
    DEBUG("TC 2: ");
    DEBUG(temp);
    DEBUG("\n");
     Serial.flush();

    // Comms::Packet p = {.id = 111};
    // Comms::packetAddFloat(&p, temp);
    // Comms::emitPacket(&p);

    return TC_UPDATE_PERIOD;
}

uint32_t tc3Sample() {
    float sensorValue = analogRead(HAL::tcAmp3_pin);
    float temp = (3.3*(sensorValue/1024)-1.25)/0.005;
    DEBUG("TC 3: ");
    DEBUG(temp);
    DEBUG("\n");
    Serial.flush();

    // Comms::Packet p = {.id = 112};
    // Comms::packetAddFloat(&p, temp);
    // Comms::emitPacket(&p);

    return TC_UPDATE_PERIOD;
}

uint32_t tc4Sample() {
    float sensorValue = analogRead(HAL::tcAmp4_pin);
    float temp = (3.3*(sensorValue/1024)-1.25)/0.005;
    temp = -1;


    DEBUG("TC 4: ");
    DEBUG(temp);
    DEBUG("\n");
    Serial.flush();

    // Comms::Packet p = {.id = 113};
    // Comms::packetAddFloat(&p, temp);
    // Comms::emitPacket(&p);

    return TC_UPDATE_PERIOD;
}

Task taskTable[] = {
    // thermocouples
    {&tc1Sample, 0},
    {&tc2Sample, 0},
    {&tc3Sample, 0},
    {&tc4Sample, 0},

    // load cells
    // {&LoadCells::sampleLoadCells, 0},
};

#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

int main() {
    // hardware setup
    Serial.begin(115200);
    #ifdef DEBUG_MODE
    while(!Serial) {} // wait for user to open serial port (debugging only)
    #endif

    DEBUG("BEFORE INIT\n");
    DEBUG("INITIALZING HAL\n");
    HAL::initHAL();
    DEBUG("INITIALIZING COMMS\n");
    // Comms::initComms();
    DEBUG("INITIALIZING LOAD CELLS\n");
    // LoadCells::initLoadCells();
    

    while(1) {
        uint32_t ticks = micros(); // current time in microseconds
        for(uint32_t i = 0; i < TASK_COUNT; i++) { // for each task, execute if next time >= current time
        if (taskTable[i].enabled && ticks >= taskTable[i].nexttime)
            // DEBUG(i);
            // DEBUG("\n");
            taskTable[i].nexttime = ticks + taskTable[i].taskCall();
        }
        // Comms::processWaitingPackets();
    }
    return 0;
}
