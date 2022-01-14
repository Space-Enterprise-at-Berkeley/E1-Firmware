#include "TCA6408A.h"

TCA6408A::TCA6408A() {}

void TCA6408A::init(uint8_t addr, TwoWire *wire) {
  _addr = addr;
  _wire = wire;

  _wire->beginTransmission(_addr);
  _wire->write(REG_CONFIG);
  _wire->write(CONFIG_ALL_OUTPUT);
  _wire->endTransmission();

  for (int i = 0; i <8; i++){
    turnOff(i);
  }
}

void TCA6408A::turnOn(uint8_t channel) {
  currOutputStatus |= (1 << channel);
  writeOutput();
}

void TCA6408A::turnOff(uint8_t channel) {
  currOutputStatus &= ~(1 << channel);
  writeOutput();
}

void TCA6408A::writeOutput() {
  _wire->beginTransmission(_addr);
  _wire->write(REG_OUTPUT_PORT);
  _wire->write(currOutputStatus);
  _wire->endTransmission();
}