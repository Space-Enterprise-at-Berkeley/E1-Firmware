#include <MAX31855.h>

int MAX31855::init(SPIClass *spi, uint8_t chipSelect)
{ // assume that numSensors is < max Size of packet. Add some error checking here
    _chipSelect = chipSelect;
    _spi = spi;

    _spi->begin();
    _spi->beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));

    pinMode(_chipSelect, OUTPUT);
    digitalWriteFast(_chipSelect, HIGH);

    return 0;
}

float MAX31855::readCelsius()
{
    int32_t v;

    digitalWriteFast(_chipSelect, LOW);
    v = spiread32();
    digitalWriteFast(_chipSelect, HIGH);

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

uint32_t MAX31855::spiread32(void)
{
    uint32_t d = 0;
    uint8_t buf[4];

    _spi->transfer(buf, 4);

    d = buf[0];
    d <<= 8;
    d |= buf[1];
    d <<= 8;
    d |= buf[2];
    d <<= 8;
    d |= buf[3];

    return d;
}
