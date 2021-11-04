#include <Arduino.h>
#include <ADS1219.h>
#include <ADS8167.h>

namespace newLoadCell
{
    void init(uint8_t numNewLoadCells, TwoWire* wire, byte digPotVal, uint8_t * adcIndices, uint8_t * adcChannels, ADC ** adcs);
    void getNewLoadCellReads(float *data);

    





    
} // namespace name
