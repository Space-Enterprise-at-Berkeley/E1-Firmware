/**
 * Device driver for TCA6408ARGTR
 */
 #ifndef __TCA6408A__
 #define __TCA6408A__

#include <Arduino.h>
#include <Wire.h>

const uint8_t TCA6408A_ADDR1 = 0x20;
const uint8_t TCA6408A_ADDR2 = 0x21;

const uint8_t REG_INPUT_PORT = 0x00;
const uint8_t REG_OUTPUT_PORT = 0x01;
const uint8_t REG_POLARITY_INVERSION = 0x02;
const uint8_t REG_CONFIG = 0x03;

//set individual bit to 0 if out, 1 if in.
const uint8_t CONFIG_ALL_OUTPUT  = 0b00000000;
const uint8_t CONFIG_ALL_INPUT = 0b11111111;

class TCA6408A {
public:
  TCA6408A();
  void init(uint8_t addr, TwoWire *wire);
  void turnOn(uint8_t channel);
  void turnOff(uint8_t channel);
private:

  void writeOutput();

  uint8_t _addr; // 0x20(Gnd) or 0x21 (high)
  int8_t _intPin;
  TwoWire *_wire;
  uint8_t currOutputStatus = 0;
};

#endif