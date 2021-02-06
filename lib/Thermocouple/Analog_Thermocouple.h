/*
  Analog_Thermocouple.h - A c++ library to interface with analog thermocouples
  via our ADCs: ADS1219.
  Created by Vamshi Balanaga, Jan 20, 2021.
*/

#ifndef __ANALOG_THERMS__
#define __ANALOG_THERMS__

#include <ADS1219.h>
#include <Wire.h>

namespace Thermocouple {
  namespace Analog {

    ADS1219 ** _adcs;

    int * _adcIndices; // array of size _numSensors
    int * _adcChannels;

    int _numSensors; // number of analog thermocouples, not number of adcs

    float tempOverVoltageScale = 1 / 0.01;
    float voltageOffset = 0.75, tempOffset = 25; // 25 C = 0.450 V

    long rawRead;
    float voltageRead;


    void init (int numSensors, int * adcIndices, int * adcChannels, ADS1219 ** adcs) {
      _numSensors = numSensors;
      _adcIndices = adcIndices;
      _adcChannels = adcChannels;
      _adcs = adcs;
    }

    // float *data is only of size 7 rn, ensure that we only expect <= 7 readings.
    void readTemperatureData(float *data) {
      int i = 0;
      while (i < _numSensors) {
        rawRead = _adcs[_adcIndices[i]]->readData(_adcChannels[i]);
        voltageRead = (float) rawRead * (5.0 / pow(2,23));
        data[i] = ((voltageRead - voltageOffset) * tempOverVoltageScale) + tempOffset;
        i++;
      }
      data[i] = -1;
    }

  }
}

#endif