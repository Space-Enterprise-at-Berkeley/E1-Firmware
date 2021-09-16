/*
  SPI_Thermocouple.h - A c++ library to interface with heavy duty cryo/high temp
  thermocouples using the MAX31855 chip.
  Created by Justin Yang, Sep 15, 2021.
*/

#ifndef __SPI_THERMS__
#define __SPI_THERMS__

#include <Adafruit_MAX31855.h>

namespace Thermocouple {
  class SPI {
    public:
      int init(uint8_t numSensors, Adafruit_MAX31855 *cryo_boards, uint8_t * chip_selects , uint8_t clk, uint8_t data_out, float *latestReads); // assume that numSensors is < max Size of packet. Add some error checking here
      void readCryoTemps(float *data);
      void readSpecificCryoTemp(uint8_t index, float *data);
      int freeAllResources();
    private:
      Adafruit_MAX31855 * _cryo_amp_boards;
      uint8_t * _chip_selects;
      uint8_t _clk;
      uint8_t _data_out;
      uint8_t _numSensors;
      float * _latestReads;
  };
}

#endif
