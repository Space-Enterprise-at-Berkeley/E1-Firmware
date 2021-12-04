#include <SPI.h>

namespace _SPI {
    SPIClass::SPIClass() {
        std::cout << "Initialize SPI Class" << "\n";
    }

    void SPIClass::begin() {
        std::cout << "Beginning SPI transmission" << "\n";
    }
}

_SPI::SPIClass SPI;
