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

  int DTRDY_PIN_1 = 29;
  int DTRDY_PIN_2 = 28;
  // int RESET_PIN_1 = 15;
  // int RESET_PIN_2 = 29;

  // first 7 bits for Arduino Wire
  uint8_t ADC1_ADDR = 0b1001010;
  uint8_t ADC2_ADDR = 0b1001000;

  TwoWire *localWire;

  ADS1219 ads1(DTRDY_PIN_1, ADC1_ADDR, localWire);
  ADS1219 ads2(DTRDY_PIN_2, ADC2_ADDR, localWire);

  uint8_t buffer[4];

  uint32_t calibration1 = 0;
  uint32_t calibration2 = 0;

  // int resetADC(int ADDR) {
  //   localWire->beginTransmission(ADDR);
  //   localWire->write(0b00000110);
  //   localWire->endTransmission();
  //   return 0;
  // }
  //
  uint32_t calibrateADC(ADS1219 ads) {
    uint64_t calibrate = 0;
    for (int i = 0; i <  10; i++){
      uint32_t temp = ads.readShorted();
      calibrate += temp;
    }
    return uint32_t (calibrate / 10);
  }

  void init(TwoWire *wire) {
    // resetADC(ADC1_ADDR);
    // resetADC(ADC2_ADDR);

    ads1.begin();
    ads2.begin();

    ads1.setConversionMode(SINGLE_SHOT);
    ads2.setConversionMode(SINGLE_SHOT);

    ads1.setVoltageReference(REF_EXTERNAL);
    ads2.setVoltageReference(REF_EXTERNAL);

    ads1.setGain(GAIN_ONE);
    ads2.setGain(GAIN_ONE);

    ads1.setDataRate(90);
    ads2.setDataRate(90);

    pinMode(DTRDY_PIN_1, INPUT_PULLUP);
    pinMode(DTRDY_PIN_2, INPUT_PULLUP);

  }


// All the following reads are blocking calls.

  void readPropaneTankPressure(float *data) {
    //AIN0
    data[0] = ads1.readData(1) - calibration1;
    data[1] = -1;
  }

  void readLOXTankPressure(float *data) {
    // AIN1
    data[0] = ads1.readData(0) - calibration1;
    data[1] = -1;
  }

  void readPropaneInjectorPressure(float *data) {
    // AIN2
    data[0] = ads1.readData(3) - calibration1;
    data[1] = -1;
  }

  void readLOXInjectorPressure(float *data) {
    //AIN3
    data[0] = ads1.readData(2) - calibration1;
    data[1] = -1;
  }

  void readPressurantTankPressure(float *data) {
    //AIN 0 on ADC 2
    data[0] = ads2.readData(0) - calibration2;
    data[1] = -1;
  }

  void readAllLowPressures(float *data) {
    data[0] = ads1.readData(0);// - calibration1;
    data[1] = ads1.readData(1);// - calibration1;
    data[2] = ads1.readData(2);// - calibration1;
    data[3] = ads1.readData(3);// - calibration1;
    // data[4] = -1;
    return;
  }

  void readAllPressures(float *data) {
    readAllLowPressures(data);
    data[4] = ads2.readData(0); //- calibration2;
    data[5] = -1;
    return;
  }


      float tempOverVoltageScale = 1 / 0.01;
      float voltageOffset = 0.75, tempOffset = 25; // 25 C = 0.450 V

      long rawRead;
      float voltageRead;
      float tempRead;

  void readTemperatureData(float *data) { // FIGURE OUT WHICH IN THIS WILL BE ON THE PCB
    //rawRead = ads2.readData(1); // thermocouple on AIN1 of ADC2
    rawRead = ads1.readData(1);
    // Serial.println(rawRead);
    // Serial.println(ads.readData(2));
    // Serial.println(ads.readData(3));
    voltageRead = (float) rawRead * (5.0 / pow(2,23));
    // Serial.println(voltageRead);
    tempRead = ((voltageRead - voltageOffset) * tempOverVoltageScale) + tempOffset;
    data[0] = tempRead;
    data[1] = -1;
  }
}

#endif
