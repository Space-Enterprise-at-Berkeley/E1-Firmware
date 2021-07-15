#include <Scheduler.h>
#include <Automation.h>
#include <Comms.h>
#include <Ducers.h>
#include <Heaters.h>
#include <Thermocouples.h>
#include <Power.h>
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
}

void loop()
{
  // run the scheduler
  Scheduler::loop();
}
