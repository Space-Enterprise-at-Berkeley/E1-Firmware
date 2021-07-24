#include <Scheduler.h>
// #include <Automation.h>
// #include <Comms.h>
// #include <Ducers.h>
// #include <Heaters.h>
// #include <Thermocouples.h>
// #include <Power.h>
// #include <Valves.h>

#include <INA219.h>

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

void hardware_init() {
  // low level hardware setup
  Serial.begin(9600);
  Wire1.begin();
  Wire1.setClock(400000); // 400khz clock
  SPI.begin();
}

int main(void) {
  hardware_init();

  while(1) {
    Scheduler::loop();
    yield();
  }
}
