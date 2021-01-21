/*
  OneWire_Thermocouple.h - A c++ library to interface with the DS18B20
  thermocouples over the OneWire protocol.
  Created by Vamshi Balanaga, Jan 20, 2021.
*/

#ifndef __OW_THERMS__
#define __OW_THERMS__

#include <OneWire.h>

namespace Thermocouple {
  namespace OW {

    int OW_DATA_PIN;

    byte sensorData[12];
    byte rom[8];

    uint8_t numThermos;
    int currSensorNum = 0;
    float celsius;
    OneWire _ow;

    void init(int numSensors, int owPinNum) {
      OW_DATA_PIN = owPinNum;
      _ow = OneWire(OW_DATA_PIN);
      numThermos = numSensors;
      _ow.reset();
      delay(500);
      _ow.search(rom);
    }

    void scrollToRightSensor(int whichSensor) {
      bool searched = false;
      if(whichSensor <= currSensorNum) {
         _ow.reset_search();
         currSensorNum = 0;
         delay(250);
         searched = true;
      }
      for (int i = currSensorNum; i <= whichSensor; i++) {
        Serial.println("searching");
        _ow.search(rom);
        currSensorNum = i;
        searched=true;
      }
      Serial.println(searched);
      if(searched) {
        _ow.reset();
        _ow.select(rom);
        _ow.write(0x44, 1);

        delay (1000);
        Serial.println("finishded 700ms wait");

        _ow.reset();
        _ow.select(rom);
        _ow.write(0xBE);
      }
      for(int i = 0; i < 8; i++){
        Serial.print(rom[i]);
      }
      Serial.println("");
    }

    /**
     * Alias for 'scrollToRightSensor'. Makes more sense to user.
    */
    void setSensor(int whichSensor){
      if(whichSensor > numThermos){
        whichSensor = 0;
      }
      scrollToRightSensor(whichSensor);
    }

    float readTemperatureData(float *data) {
      for (int i = 0; i < 8; i++) { // need 9 bytes apparently.
        sensorData[i] = _ow.read();
      }
      int16_t raw = (sensorData[1] << 8) | sensorData[0];
      celsius = (float)raw / 16.0;
      //Serial.println(celsius);
      data[0] = celsius;
      data[1] = -1;
      Serial.print("celsius: ");
      Serial.println(celsius);
      return celsius;
    }

  }
}

#endif
