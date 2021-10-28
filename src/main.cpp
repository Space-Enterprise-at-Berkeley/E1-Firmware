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

#include <tests/TestHeaters.h>
#include <tests/TestPower.h>

TestPower::testReadouts *testPwr;
TestPower::testPowerTasks *testPwrTasks;
TestHeater::testFunc *testHtr;

void setup()
{
  // low level hardware setup
  Wire1.begin();
  Wire1.setClock(400000); // 400khz clock
  SPI.begin();

  // initialize testing functions
  testPwr = new TestPower::testReadouts();
  testPwrTasks = new TestPower::testPowerTasks();
  testHtr = new TestHeater::testFunc();
}

void loop()
{
  // run the scheduler
  Scheduler::loop();
  // run Tests - uncomment only one at a time!
  testPwr->loop();
  // testPwrTasks->loop();
  // testHtr->loop();
}
