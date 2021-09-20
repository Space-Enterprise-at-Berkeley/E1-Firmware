/*
  SPI_Thermocouple.h - A c++ library to interface with heavy duty cryo/high temp
  thermocouples using the MAX31855 chip.
  Created by Justin Yang, Sep 15, 2021.
*/

#include <SPI_Thermocouple.h>
#include <SPI.h>

namespace Thermocouple
{

  int SPI_TC::init(uint8_t numSensors, uint8_t *chip_selects, float *latestReads)
  { // assume that numSensors is < max Size of packet. Add some error checking here
    _chip_selects = chip_selects;
    _latestReads = latestReads;

    _numSensors = numSensors;

    SPI.begin();
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));

    for (int i = 0; i < _numSensors; i++)
    {
      pinMode(_chip_selects[i], OUTPUT);
      digitalWrite(_chip_selects[i], HIGH);
      Serial.println(i);
      Serial.println(_latestReads[i]);
      _latestReads[i] = -1;
      Serial.println(_latestReads[i]);
      _latestReads[i] = readCelsius(_chip_selects[i]);
      Serial.println(_latestReads[i]);
    }

    return 0;
  }

  void SPI_TC::readCryoTemps(float *data)
  {
    #ifdef DEBUG
    Serial.println("read Cryo temps");
    Serial.flush();
    #endif
    for (int i = 0; i < _numSensors; i++)
    {
      #ifdef DEBUG
      Serial.print(i);
      Serial.flush();
      #endif
      data[i] = readCelsius(_chip_selects[i]);
      #ifdef DEBUG
      Serial.print(data[i]);
      Serial.flush();
      #endif
      _latestReads[i] = data[i];
    }
    data[_numSensors] = -1;
  }

  void SPI_TC::readSpecificCryoTemp(uint8_t index, float *data)
  {
    data[0] = _latestReads[index];
    data[1] = -1;
  }

  int SPI_TC::freeAllResources()
  {
    free(_latestReads);
    return 0;
  }

  float SPI_TC::readCelsius(uint8_t cs)
  {
    int32_t v;

    digitalWrite(cs, LOW);
    v = spiread32();
    digitalWrite(cs, HIGH);

    if (v & 0x7)
    {
      return NAN;
    }

    if (v & 0x80000000)
    {
      // Negative value, drop the lower 18 bits and explicitly extend sign bits.
      v = 0xFFFFC000 | ((v >> 18) & 0x00003FFF);
    }
    else
    {
      // Positive value, just drop the lower 18 bits.
      v >>= 18;
    }

    double centigrade = v;

    // LSB = 0.25 degrees C
    centigrade *= 0.25;
    return centigrade;
  }

  uint32_t SPI_TC::spiread32(void)
  {
    uint32_t d = 0;
    uint8_t buf[4];

    SPI.transfer(buf, 4);

    d = buf[0];
    d <<= 8;
    d |= buf[1];
    d <<= 8;
    d |= buf[2];
    d <<= 8;
    d |= buf[3];

    // Serial.println(d, HEX);

    return d;
  }

}
