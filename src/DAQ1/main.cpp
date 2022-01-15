#include <Common.h>
#include <Comms.h>
#include "HAL.h"
#include "ADC.h"
#include "Thermocouples.h"
#include "LoadCells.h"

#include <Arduino.h>

Task taskTable[] = {
    // adc
    {&ADC::adcSample, 0},

    // thermocouples
    {&Thermocouples::tc0Sample, 0},
    {&Thermocouples::tc1Sample, 0},
    {&Thermocouples::tc2Sample, 0},
    {&Thermocouples::tc3Sample, 0},

    // load cells
    {&LoadCells::sampleLoadCells, 0},
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
    ADC::initADC();
    Thermocouples::initThermocouples();
    LoadCells::initLoadCells();

    while(1) {
        uint32_t ticks = micros(); // current time in microseconds
        for(uint32_t i = 0; i < TASK_COUNT; i++) { // for each task, execute if next time >= current time
        if (taskTable[i].enabled && ticks >= taskTable[i].nexttime)
            taskTable[i].nexttime = ticks + taskTable[i].taskCall();
        }
        Comms::processWaitingPackets();
    }
    return 0;
}
