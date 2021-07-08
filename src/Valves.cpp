#include <Valves.h>

namespace Valves
{
  // Valve class definitions
  Valve::Valve(uint8_t pin,
               Voltage *vSense,
               Current *cSense) : pin(pin),
                                  state(false),
                                  vSense(vSense),
                                  cSense(cSense) {}

  void Valve::init()
  {
    pinMode(pin, OUTPUT);
  }

  bool Valve::open()
  {
    digitalWriteFast(pin, HIGH);
    bool didChange = !state; // if valve was already on, did not change
    state = true;
    return didChange;
  }

  bool Valve::close()
  {
    digitalWriteFast(pin, LOW);
    bool didChange = state; // if valve was already off, did not change
    state = false;
    return didChange;
  }

  void Valve::run()
  {
  }

  // sensor definitions
  TwoWire *senseWire = &Wire1;
  const float senseShuntR = 0.033;   // ohms
  const float senseMaxCurrent = 4.0; // amps

  INA219 armSense;
  INA219 loxMainSense;
  INA219 propMainSense;

  // telemetry definitions
  Voltage armVoltage(&armSense);
  Voltage loxMainVoltage(&loxMainSense);
  Voltage propMainVoltage(&propMainSense);

  Current armCurrent(&armSense);
  Current loxMainCurrent(&loxMainSense);
  Current propMainCurrent(&propMainSense);

  // valve definitions
  Valve arm(5, &armVoltage, &armCurrent);
  Valve loxMain(3, &loxMainVoltage, &loxMainCurrent);
  Valve propMain(2, &propMainVoltage, &propMainCurrent);

  void configureINA219(INA219 sensor, uint8_t address)
  {
    sensor.begin(senseWire, address);
    sensor.configure(INA219_RANGE_32V,
                     INA219_GAIN_160MV);
    sensor.calibrate(senseShuntR,
                     senseMaxCurrent);
  }

  void init()
  {
    configureINA219(armSense, 0x40);
    configureINA219(loxMainSense, 0x42);
    configureINA219(propMainSense, 0x43);

    Scheduler::repeatTask(armVoltage, 200 * 1000);
    Scheduler::repeatTask(loxMainVoltage, 200 * 1000);
    Scheduler::repeatTask(propMainVoltage, 200 * 1000);

    Scheduler::repeatTask(armCurrent, 100 * 1000);
    Scheduler::repeatTask(loxMainCurrent, 100 * 1000);
    Scheduler::repeatTask(propMainCurrent, 100 * 1000);

    Scheduler::repeatTask(&arm, 100 * 1000);
    Scheduler::repeatTask(&loxMain, 100 * 1000);
    Scheduler::repeatTask(&propMain, 100 * 1000);

    arm.init();
    loxMain.init();
    propMain.init();
  }
};
