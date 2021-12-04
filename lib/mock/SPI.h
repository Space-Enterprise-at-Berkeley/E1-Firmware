#pragma once

#include <iostream>

namespace _SPI {
  class SPIClass {
    public:
    SPIClass();
    void begin();
  };
}

extern _SPI::SPIClass SPI;