#pragma once

#include <cstdint>

class TwoWire {
public:
  TwoWire();
  void begin();
  void setClock(uint32_t baud);
};

extern TwoWire Wire;
extern TwoWire Wire1;
