/*
  Cryo_Thermocouple.cpp - A c++ library to interface with heavy duty cryo/high temp
  thermocouples using the MCP9600 chip.
  Created by Vamshi Balanaga, Feb 5, 2021.
*/



#include <Cryo_Thermocouple.h>

namespace Thermocouple {

    int Cryo::init(int numSensors, Adafruit_MCP9600 *cryo_boards, int * addrs, _themotype * types, TwoWire *theWire) { // assume that numSensors is < max Size of packet. Add some error checking here
      _addrs = addrs;
      _latestReads = (float *)malloc(numSensors);
      _cryo_amp_boards = cryo_boards;

      _numSensors = numSensors;

      for (int i = 0; i < numSensors; i++) {

        if (!_cryo_amp_boards[i].begin(addrs[i], theWire)) {
          Serial.println("Error initializing cryo board at Addr 0x" + String(addrs[i], HEX));
          return -1;
        }

        _cryo_amp_boards[i].setADCresolution(MCP9600_ADCRESOLUTION_12);
        _cryo_amp_boards[i].setThermocoupleType(types[i]);
        _cryo_amp_boards[i].setFilterCoefficient(3);
        _cryo_amp_boards[i].enable(true);
      }

      return 0;
    }

    void Cryo::readCryoTemps(float *data) {
      for (int i = 0; i < _numSensors; i++) {
        data[i] = _cryo_amp_boards[i].readThermocouple();
        _latestReads[i] = data[i];
      }
      data[_numSensors] = -1;
    }


    void Cryo::readSpecificCryoTemp(int index, float *data) {
      data[0] = _latestReads[index];
      data[1] = -1;
    }

    int Cryo::freeAllResources() {
        free(_latestReads);
        return 0;
    }

  }
