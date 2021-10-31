/*
  ducer.h - A c++ library to interface with the analog pressure transducers via the
  Created by Vamshi Balanaga, Sept 18, 2020.
*/

#ifndef __DUCERS__
#define __DUCERS__

#include <Arduino.h>
#include <ADS1219.h>
#include <ADS8167.h>
#include <cmath>

using namespace std;

namespace Ducers {

  ADC ** _adcs;

  uint8_t * _adcIndices; // array of size _numSensors
  uint8_t * _adcChannels;
  uint32_t * _ptTypes;

  float * _latestReads;

  uint8_t _numInitialOffsetReads;
  float * _offsets;
  uint8_t * _offsetCounters; //array of size numSensors, each element counts down from numInitialOffsetReads to zero
                            // to keep track of how many reads to average
  const float offset_threshold = 10.0; //threshold to determine if a PT just has an offset or is actually pressurized

  const uint8_t strideLength = sizeof(ADS8167);

  uint8_t _numSensors; // number of analog thermocouples, not number of adcs

  uint8_t buffer[4];

  void init (uint8_t numSensors, uint8_t * adcIndices, uint8_t * adcChannels, uint32_t * ptTypes, ADC ** adcs, uint8_t numInitialOffsetReads) {
    _numSensors = numSensors;
    _adcIndices = adcIndices;
    _adcChannels = adcChannels;
    _ptTypes = ptTypes;
    _adcs = adcs;
    _latestReads = (float *)malloc(numSensors);

    _numInitialOffsetReads = numInitialOffsetReads;
    _offsets = (float *)malloc(numSensors); //actually stores the offsets
    _offsetCounters = (uint8_t *) malloc(numSensors); //a countdown to how many more readings to collect before averaging. 
    //Not using one counter for everything since some PTs might not be at ambient pressure while others might be

    
    for (int i = 0; i < _numSensors; i ++){
      Serial.println(_ptTypes[i]);
      _offsets[i] = 0.0;
      _offsetCounters[i] = _numInitialOffsetReads;
    }
  }

  /*
    lox    : Static Pressure=-1.158+1.029*Dome Pressure-0.02228*High Pressure
    propane: Static Pressure=-20.08+1.413*Dome Pressure+0.002343*High Pressure
  */

  float loxStaticP(float loxDomeP, float highP) {
    return -1.158 + 1.029 * loxDomeP - 0.02228 * highP;
  }

  float propStaticP(float propDomeP, float highP) {
    return -20.08 + 1.413 * propDomeP + 0.002343 * highP;
  }

  float interpolateHigh(long rawValue) { //5k psi sensor
      double values[2][2] =  {
                { 0, 0 },
                { 64850, 5000 }
    };

    // return std::lerp(0.0, 5000.0, (double)rawValue/64850);
    float upperBound = values[1][0];
    float lowerBound = values[0][0];
    float upperBoundPressure = values[1][1];
    float lowerBoundPressure = values[0][1];
    float proportion = (rawValue - lowerBound)/(upperBound - lowerBound);
    float convertedValue = proportion * (upperBoundPressure - lowerBoundPressure) + lowerBoundPressure;
    return convertedValue;
  }

  float interpolateLow(long rawValue) { // 1k psi sensor
    double values[2][2] = { // [x, y] pairs
                {0, -123.89876445934394},
                {64901, 1131.40825} // 2^23 - 1
              };
    // return std::lerp(-123.89876445934394, 1131.40825, (double) rawValue / 64901);
    float upperBound = values[1][0];
    float lowerBound = values[0][0];
    float upperBoundPressure = values[1][1];
    float lowerBoundPressure = values[0][1];
    float proportion = (rawValue - lowerBound)/(upperBound - lowerBound);
    float convertedValue = proportion * (upperBoundPressure - lowerBoundPressure) + lowerBoundPressure;
    return convertedValue;
  }


  float interpolate(long rawValue, int ptType) { // 300 psi sensor
    double values[2][2] = { // [x, y] pairs
      {6553,  0},
      {58982, (double) ptType}
    };
    // return std::lerp(-123.89876445934394, 1131.40825, (double) rawValue / 64901);
    float upperBound = values[1][0];
    float lowerBound = values[0][0];
    float upperBoundPressure = values[1][1];
    float lowerBoundPressure = values[0][1];
    float proportion = (rawValue - lowerBound)/(upperBound - lowerBound);
    float convertedValue = proportion * (upperBoundPressure - lowerBoundPressure) + lowerBoundPressure;
    return convertedValue;
  }

  float interpolate300(long rawValue) { // 300 psi sensor
    double values[2][2] = { // [x, y] pairs
      {6553,  0},
      {58982, 300}
    };
    // return std::lerp(-123.89876445934394, 1131.40825, (double) rawValue / 64901);
    float upperBound = values[1][0];
    float lowerBound = values[0][0];
    float upperBoundPressure = values[1][1];
    float lowerBoundPressure = values[0][1];
    float proportion = (rawValue - lowerBound)/(upperBound - lowerBound);
    float convertedValue = proportion * (upperBoundPressure - lowerBoundPressure) + lowerBoundPressure;
    return convertedValue;
  }

  float interpolate100(long rawValue) { // 100 psi sensor
    double values[2][2] = { // [x, y] pairs
                {6553,  0},
                {58982, 100}
              };
    // return std::lerp(-123.89876445934394, 1131.40825, (double) rawValue / 64901);
    float upperBound = values[1][0];
    float lowerBound = values[0][0];
    float upperBoundPressure = values[1][1];
    float lowerBoundPressure = values[0][1];
    float proportion = (rawValue - lowerBound)/(upperBound - lowerBound);
    float convertedValue = proportion * (upperBoundPressure - lowerBoundPressure) + lowerBoundPressure;
    return convertedValue;
  }

  float interpolate30(long rawValue) { // 100 psi sensor
    double values[2][2] = { // [x, y] pairs
                {6553,  0},
                {58982, 30}
              };
    // return std::lerp(-123.89876445934394, 1131.40825, (double) rawValue / 64901);
    float upperBound = values[1][0];
    float lowerBound = values[0][0];
    float upperBoundPressure = values[1][1];
    float lowerBoundPressure = values[0][1];
    float proportion = (rawValue - lowerBound)/(upperBound - lowerBound);
    float convertedValue = proportion * (upperBoundPressure - lowerBoundPressure) + lowerBoundPressure;
    return convertedValue;
  }


  // All the following reads are blocking calls.
  void readAllPressures(float *data) {
    for (int i = 0; i < _numSensors; i++) {
      int type = _ptTypes[i];
      if (type == 1000) {
        #ifdef DEBUG
          Serial.println("reading low pressure data from ADC" + String(_adcIndices[i]) + " Ain" + String(_adcChannels[i]));
          Serial.flush();
        #endif
        data[i] = interpolateLow(_adcs[_adcIndices[i]]->readData(_adcChannels[i]));
      } else if (type == 5000) {
        #ifdef DEBUG
          Serial.println("reading high pressure data from ADC" + String(_adcIndices[i]) + " Ain" + String(_adcChannels[i]));
          Serial.flush();
        #endif
        data[i] = interpolateHigh(_adcs[_adcIndices[i]]->readData(_adcChannels[i]));
        //data[i] = _adcs[_adcIndices[i]]->readData(_adcChannels[i]);
      } else if (type == 100) {
        #ifdef DEBUG
          Serial.println("reading 100 pressure data from ADC" + String(_adcIndices[i]) + " Ain" + String(_adcChannels[i]));
          Serial.flush();
        #endif
        data[i] = interpolate100(_adcs[_adcIndices[i]]->readData(_adcChannels[i]));
      } else if (type == 300) {
        #ifdef DEBUG
          Serial.println("reading 300 pressure data from ADC" + String(_adcIndices[i]) + " Ain" + String(_adcChannels[i]));
          Serial.flush();
        #endif
        data[i] = interpolate300(_adcs[_adcIndices[i]]->readData(_adcChannels[i]));
      } else if (type == 30) {
        #ifdef DEBUG
          Serial.println("reading 30 pressure data from ADC" + String(_adcIndices[i]) + " Ain" + String(_adcChannels[i]));
          Serial.flush();
        #endif
        data[i] = interpolate30(_adcs[_adcIndices[i]]->readData(_adcChannels[i]));
      } else if (type == 150) {
        #ifdef DEBUG
          Serial.println("reading 30 pressure data from ADC" + String(_adcIndices[i]) + " Ain" + String(_adcChannels[i]));
          Serial.flush();
        #endif
        data[i] = interpolate(_adcs[_adcIndices[i]]->readData(_adcChannels[i]), type);
      }

      if (_offsetCounters[i] == 0) {
        _latestReads[i] = data[i] - _offsets[i];
      } else {

        if (data[i] > offset_threshold) { //if its pressurized above threshold, stop trying to offset it & nuke all existing offsets/counters
          _offsetCounters[i] = _numInitialOffsetReads;
          _offsets[i] = 0.0;
        } else { //its at something below threshold, so decrease counter and add the value
          _offsetCounters[i] -= 1; //decrease the counter, add value to the offset
          _offsets[i] += data[i];
          if (_offsetCounters[i] == 0) { //average the offset if the counter goes to zero
            _offsets[i] /= _numInitialOffsetReads;
          }
        }

        _latestReads[i] = data[i];

        
        

      }

    }
    data[_numSensors] = -1;
  }
}

#endif
