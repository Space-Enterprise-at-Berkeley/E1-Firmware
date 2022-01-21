#pragma once

#include <Arduino.h>
#include <SPI.h>

class MAX31855 {
    public:
        int init(SPIClass *spi, uint8_t chipSelect); // assume that numSensors is < max Size of packet. Add some error checking here
        float readCelsius();
    private:
        uint32_t spiread32(void);
        uint8_t _chipSelect;
        SPIClass *_spi;
};