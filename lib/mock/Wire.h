#pragma once

#include <cstdint>
#include <math.h>

class TwoWire {
  public:
  TwoWire();
  void begin();
  void setClock(uint32_t baud);
  void beginTransmission(uint8_t address);
  int write(uint8_t value);
  uint8_t endTransmission();
  void send(uint8_t value);
  uint8_t requestFrom(uint8_t, uint8_t);
  uint8_t available();
  uint8_t receive();
  uint8_t read();
};

extern TwoWire Wire;
extern TwoWire Wire1;
