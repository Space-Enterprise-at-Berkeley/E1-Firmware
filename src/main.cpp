// #include <Scheduler.h>
// #include <Automation.h>
// #include <Comms.h>
// #include <Ducers.h>
// #include <Heaters.h>
// #include <Thermocouples.h>
// #include <Power.h>
// #include <Valves.h>

#include <Arduino.h>
// #include <Wire.h>
// #include <SPI.h>

extern int main (void);

int main(void) {
  // low level hardware setup
  Serial.begin(9600);
  // Wire1.begin();
  // Wire1.setClock(400000); // 400khz clock
  // SPI.begin();
  pinMode(1, OUTPUT);

  while(1) {
    // digitalWriteFast(1, HIGH);
    // delay(1000);
    // digitalWriteFast(1, LOW);
    Serial.println("test");
    delay(1000);
    // Scheduler::loop();
    // yield();
  }
}
