#include <newLoadCell.h>

using namespace std;
namespace newLoadCell
{
    uint8_t _digPotVal, _numNewLoadCells;


    ADC **_adcs;

    uint8_t * _adcIndices; // array of size _numSensors
    uint8_t * _adcChannels;
    float * _loadCellScaling;
    float * _offsets;

    void init(uint8_t numNewLoadCells, uint8_t loadCellDigPotAddr, TwoWire* wire, float * loadCellScaling, byte digPotVal, uint8_t * adcIndices, uint8_t * adcChannels, ADC ** adcs, float* offsets)
    {
        _adcIndices = adcIndices;
        _adcChannels = adcChannels;
        _adcs = adcs;
        _numNewLoadCells = numNewLoadCells;
        _loadCellScaling = loadCellScaling;
        _digPotVal = digPotVal;
        _offsets = offsets;
        // wire->beginTransmission(46);
        // wire->write(byte(0x00));
        // wire->write(byte(40));
        // wire->endTransmission();

    }
    void getNewLoadCellReads(float* data) {
            
   
        float totalThrust = 0;
        for (int i = 0; i < _numNewLoadCells; i++) {
            data[i] = _loadCellScaling[i] * (_adcs[_adcIndices[i]]->readData(_adcChannels[i]) - _offsets[i]);
            totalThrust += data[i];


        }
        data[_numNewLoadCells] = totalThrust;
        data[_numNewLoadCells + 1] = -1;
    }

} // namespace newLoadCell