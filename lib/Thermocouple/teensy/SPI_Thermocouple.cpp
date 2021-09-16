/*
  Cryo_Thermocouple.cpp - A c++ library to interface with heavy duty cryo/high temp
  thermocouples using the MCP9600 chip.
  Created by Vamshi Balanaga, Feb 5, 2021.
*/

#include <SPI_Thermocouple.h> 

namespace Thermocouple {

    int SPI::init(uint8_t numSensors, Adafruit_MAX31855 *cryo_boards, uint8_t * chip_selects , uint8_t clk, uint8_t data_out, float *latestReads) { // assume that numSensors is < max Size of packet. Add some error checking here
      _chip_selects = chip_selects;
      _clk = clk;
      _data_out = data_out;
      _latestReads = latestReads;
      _cryo_amp_boards = cryo_boards;

      _numSensors = numSensors;

      for (int i = 0; i < numSensors; i++) {
        _cryo_amp_boards[i] = Adafruit_MAX31855(clk, chip_selects[i], data_out);
        if (_cryo_amp_boards[i].begin()) {
          Serial.println("Error initializing cryo board at CS#" + String(chip_selects[i]));
          return -1;
        }
      }

      for (int i = 0; i < _numSensors; i++) {
        Serial.println(i);
        Serial.println(_latestReads[i]);
        _latestReads[i] = -1;
        Serial.println(_latestReads[i]);
        _latestReads[i] = _cryo_amp_boards[i].readCelsius();
        Serial.println(_latestReads[i]);
      }

      return 0;
    }

    void SPI::readCryoTemps(float *data) {
      #ifdef DEBUG
      Serial.println("read Cryo temps");
      Serial.flush();
      #endif
      for (int i = 0; i < _numSensors; i++) {
        #ifdef DEBUG
        Serial.print(i);
        Serial.flush();
        #endif
        data[i] = _cryo_amp_boards[i].readCelsius();
        // data[i] = _cryo_amp_boards[i].readAmbient();
        // data[i] = _cryo_amp_boards[i].readADC();
        #ifdef DEBUG
        Serial.print(data[i]);
        Serial.flush();
        #endif
        _latestReads[i] = data[i];
      }
      data[_numSensors] = -1;
    }

    void SPI::readSpecificCryoTemp(uint8_t index, float *data) {
      data[0] = _latestReads[index];
      data[1] = -1;
    }

    int SPI::freeAllResources() {
        free(_latestReads);
        return 0;
    }

  }
