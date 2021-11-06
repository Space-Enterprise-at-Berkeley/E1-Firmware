#include <Arduino.h>
#include <ADS1219.h>
#include <ADS8167.h>
#include <stdio.h>

namespace newLoadCell
{
    void init(uint8_t numNewLoadCells, uint8_t loadCellDigPotAddr, TwoWire* wire, float * loadCellScaling, byte digPotVal, uint8_t * adcIndices, uint8_t * adcChannels, ADC ** adcs);
    void getNewLoadCellReads(float *data);



    





    
} // namespace name