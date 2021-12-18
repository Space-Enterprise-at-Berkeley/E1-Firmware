#include <Task.h>
#include <Comms.h>
#include <Ducers.h>
#include <HAL.h>

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

Task taskTable[] = {
  // ducers
  {Ducers::ptSample, 0},
};

#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

int main() {
  // hardware setup
  Serial.begin(115200);
  while(!Serial) {} // wait for user to open serial port (debugging only)
  HAL::initHAL();
  Ducers::initDucers();

  while(1) {
    uint32_t ticks = micros(); // current time in microseconds
    for(uint32_t i = 0; i < TASK_COUNT; i++) { // for each task, execute if next time >= current time
      if (ticks >= taskTable[i].nexttime)
        taskTable[i].nexttime = ticks + taskTable[i].taskCall();
    }
    if (Comms::packetWaiting())
      Comms::processPackets();
  }
  return 0;
}
