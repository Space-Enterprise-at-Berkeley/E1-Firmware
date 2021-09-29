/*
  SPI_Thermocouple.h - A c++ library to interface with heavy duty cryo/high temp
  thermocouples using the MAX31855 chip.
  Created by Justin Yang, Sep 15, 2021.
*/

#ifndef __SPI_THERMS__
#define __SPI_THERMS__

#include <stdint.h>

namespace Thermocouple {
  class SPI_TC {
    public:
      int init(uint8_t numSensors,  uint8_t * chip_selects , float *latestReads); // assume that numSensors is < max Size of packet. Add some error checking here
      void readCryoTemps(float *data);
      void readSpecificCryoTemp(uint8_t index, float *data);
      int freeAllResources();
    private:
      float readCelsius(uint8_t cs);
      uint32_t spiread32(void);
      uint8_t * _chip_selects;
      uint8_t _numSensors;
      float * _latestReads;
  };
}

#endif
