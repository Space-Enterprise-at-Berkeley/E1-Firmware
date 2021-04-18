
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

  for (int i = 0; i <8; i++){
    turnOff(i);
  }
}

void GpioExpander::turnOn(uint8_t channel) {
  currOutputStatus |= (1 << channel);
  writeOutput();
}

void GpioExpander::turnOff(uint8_t channel) {
  currOutputStatus &= ~(1 << channel);
  writeOutput();
}

void GpioExpander::writeOutput() {
  _wire->beginTransmission(_addr);
  _wire->write(REG_OUTPUT_PORT);
  _wire->write(currOutputStatus);
  _wire->endTransmission();
}
