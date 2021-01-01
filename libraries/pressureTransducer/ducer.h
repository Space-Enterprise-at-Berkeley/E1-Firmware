/*
  ducer.h - A c++ library to interface with the analog pressure transducers via the
  Created by Vamshi Balanaga, Sept 18, 2020.
*/

#ifndef __DUCERS__
#define __DUCERS__

#include <Arduino.h>
#include <Wire.h>
#include <ADS1219.h>
// #include <TwoWire.h>

using namespace std;

namespace Ducers {

  ADS1219 ** _adcs;

  int * _adcIndices; // array of size _numSensors
  int * _adcChannels;

  int _numSensors; // number of analog thermocouples, not number of adcs

  uint8_t buffer[4];

  void init (int numSensors, int * adcIndices, int * adcChannels, ADS1219 ** adcs) {
    _numSensors = numSensors;
    _adcIndices = adcIndices;
    _adcChannels = adcChannels;
    _adcs = _adcs;
  }

  // All the following reads are blocking calls.
  // this function takes ~ _numSensors * 15ms given the data rate of 90
  void readAllPressures(float *data) {
    int i = 0;
    while (i < _numSensors) {
      Serial.println(_adcIndices[i]);
      Serial.println(_adcChannels[i]);
      //data[i] = _adcs[_adcIndices[i]]->readData(_adcChannels[i]);
      data[i] = _adcs[0]->readData(i%4);
      Serial.println(data[i]);
      i++;
    }
    data[i] = -1;
  }
}

#endif
