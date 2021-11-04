#include <newLoadCell.h>


namespace newLoadCell
{
    uint8_t _digPotVal, _numNewLoadCells;


    ADC ** _adcs;

    uint8_t * _adcIndices; // array of size _numSensors
    uint8_t * _adcChannels;
    float* _loadCellScaling;

    void init(uint8_t numNewLoadCells, float* loadCellScaling, byte digPotVal, TwoWire* wire, uint8_t * adcIndices, uint8_t * adcChannels, ADC ** adcs);
        
        _adcIndices = adcIndices;
        _adcChannels = adcChannels;
        _adcs = adcs;

        _loadCellScaling = loadCellScaling;
        _digPotVal = digPotVal;
        _newLoadCellPins = newLoadCellPins;
        wire.beginTransmission(loadCellDigPotAddr);
        wire.write(byte(0x00));
        wire.write(byte(digPotVal));
        wire.endTransmission();

    }
    void getNewLoadCellReads(float* data) {
        float totalThrust = 0;
        for (int i = 0; i < _numNewLoadCells; i++) {
            data[i] = _loadCellScaling[i] * _adcs[_adcIndices[i]]->readData(_adcChannels[i]);
            totalThrust += data[i];


        }
        data[_numNewLoadCells] = totalThrust;
    }

} // namespace newLoadCell
