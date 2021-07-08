#pragma once

// lib imports
#include <INA219.h>
#include <Scheduler.h>
// arduino related imports
#include <Arduino.h>
// std imports
#include <vector>

using namespace std;

namespace Valves
{
  class Valve : public Task
  {
  private:
    uint8_t pin;   // GPIO pin that turns the valve on and off
    bool state;    // the current state of the valve
    Voltage *vSense; // the voltage telemetry for this valve
    Current *cSense; // the current telemetry for this valve
  public:
    Valve(uint8_t pin, Voltage *vSense, Current *cSense);
    void init(); // initialize GPIO pin
    bool open();  // open the valve; returns true if the state changed
    bool close(); // close the valve; returns true if the state changed
    void run();
  };

  // sensor definitions
  extern TwoWire* senseWire;
  extern const float senseShuntR; // shunt resistance
  extern const float senseMaxCurrent; // max expected current

  extern INA219 armSense;
  extern INA219 loxMainSense;
  extern INA219 propMainSense;

  // telemetry definitions
  extern Voltage armVoltage;
  extern Voltage loxMainVoltage;
  extern Voltage propMainVoltage;

  extern Current armCurrent;
  extern Current loxMainCurrent;
  extern Current propMainCurrent;

  // valve definitions
  extern Valve arm;
  extern Valve loxMain;
  extern Valve propMain;

  // function to initialize one INA219
  void configureINA219(INA219 sensor, uint8_t address);
  // function to initialize all valves
  void init();
};
