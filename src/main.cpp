#include <Scheduler.h>
#include <Valves.h>

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

void setup()
{
  // low level hardware setup
  Wire1.begin();
  Wire1.setClock(400000); // 400khz clock
  SPI.begin();

  // initialize valves
  Valves::init();
}

void loop()
{
  // read PT channels @ 5 kHz
  // read valves sequentially @ 5kHz
  Scheduler::loop();
}
