
#include "GpioExpander.h"

GpioExpander::GpioExpander(uint8_t addr, int8_t intPin, TwoWire *wire) {
  _addr = addr;
  _intPin = intPin;
  _wire = wire;
  _wire->begin();
}

void GpioExpander::init() {
  _wire->beginTransmission(_addr);
  _wire->write(REG_CONFIG);
  _wire->write(CONFIG_ALL_OUTPUT);
  _wire->endTransmission();
}

void GpioExpander::turnOn(uint8_t channel) {
  currOutputStatus |= (1 << channel);
}

void GpioExpander::turnOff(uint8_t channel) {
  currOutputStatus &= !(1 << channel);
}

void GpioExpander::writeOutput() {
  _wire->beginTransmission(_addr);
  _wire->write(REG_OUTPUT_PORT);
  _wire->write(currOutputStatus);
  _wire->endTransmission();
}
