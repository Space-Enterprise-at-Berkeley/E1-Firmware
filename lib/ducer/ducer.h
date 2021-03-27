/*
  ducer.h - A c++ library to interface with the analog pressure transducers via the
  Created by Vamshi Balanaga, Sept 18, 2020.
*/

#ifndef __DUCERS__
#define __DUCERS__

#include <Arduino.h>
#include <ADS1219.h>
#include <ADS8167.h>


using namespace std;

namespace Ducers {

  ADC ** _adcs;

  uint8_t * _adcIndices; // array of size _numSensors
  uint8_t * _adcChannels;
  uint8_t * _ptTypes;

  const uint8_t strideLength = sizeof(ADS8167);

  uint8_t _numSensors; // number of analog thermocouples, not number of adcs

  uint8_t buffer[4];

  void init (uint8_t numSensors, uint8_t * adcIndices, uint8_t * adcChannels, uint8_t * ptTypes, ADC ** adcs) {
    _numSensors = numSensors;
    _adcIndices = adcIndices;
    _adcChannels = adcChannels;
    _ptTypes = ptTypes;
    _adcs = adcs;
  }

  // void init (int numSensors, int * adcIndices, int * adcChannels, int * ptTypes, ADS8167 ** adcs) {
  //   _numSensors = numSensors;
  //   _adcIndices = adcIndices;
  //   _adcChannels = adcChannels;
  //   _ptTypes = ptTypes;
  //   _adcs = adcs;
  // }

  float interpolateHigh(long rawValue) {
      double values[2][2] =  {
                { 0, 0 },
                { 64850, 5000 },
    };
    bool check = true;
    int index = 0;
    while(check){
      if(rawValue > values[index][0]){index++;}
      else if (rawValue < values[0][0]){return 0;}

        check = false;
      }
      float upperBound = values[index][0];
      float lowerBound = values[index-1][0];
      float upperBoundPressure = values[index][1];
      float lowerBoundPressure = values[index-1][1];
      float proportion = (rawValue - lowerBound)/(upperBound - lowerBound);
      float convertedValue = proportion * (upperBoundPressure - lowerBoundPressure) + lowerBoundPressure;
        //float convertedValue = lerp(values[index-1][1], values[index][1], proportion);

      return convertedValue;
  }

  float interpolateLow(long rawValue) {
    double values[2][2] = { // [x, y] pairs
                {0, -123.89876445934394},
                {64901, 1131.40825} // 2^23 - 1
              };
    float upperBound = values[1][0];
    float lowerBound = values[0][0];
    float upperBoundPressure = values[1][1];
    float lowerBoundPressure = values[0][1];
    float proportion = (rawValue - lowerBound)/(upperBound - lowerBound);
    float convertedValue = proportion * (upperBoundPressure - lowerBoundPressure) + lowerBoundPressure;
    return convertedValue;
  }
  // All the following reads are blocking calls.
  // this function takes ~ _numSensors * 15ms given the data rate of 90
  void readAllPressures(float *data) {
    int i = 0;
    while (i < _numSensors) {
      int type = _ptTypes[i];
      if (type == 1) {
        #ifdef DEBUG
          Serial.println("reading low pressure data from ADC" + String(_adcIndices[i]) + " Ain" + String(_adcChannels[i]));
          Serial.flush();
        #endif
        data[i] = interpolateLow(_adcs[_adcIndices[i]]->readData(_adcChannels[i]));
        i++;
      } else {
        #ifdef DEBUG
          Serial.println("reading high pressure data from ADC" + String(_adcIndices[i]) + " Ain" + String(_adcChannels[i]));
          Serial.flush();
        #endif
        // data[i] = interpolateHigh(_adcs[_adcIndices[i]]->readData(_adcChannels[i]));
        data[i] = _adcs[_adcIndices[i]]->readData(_adcChannels[i]);
        i++;
      }
    }
    data[i] = -1;
  }
}

#endif
