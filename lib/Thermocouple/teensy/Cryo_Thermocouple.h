/*
  Cryo_Thermocouple.h - A c++ library to interface with heavy duty cryo/high temp
  thermocouples using the MCP9600 chip.
  Created by Vamshi Balanaga, Jan 20, 2021.
*/

#ifndef __CRYO_THERMS__
#define __CRYO_THERMS__

#include <Adafruit_MCP9600.h>

namespace Thermocouple {
  namespace Cryo {

    Adafruit_MCP9600 ** _cryo_amp_boards;
    int * _addrs;
    int _numSensors;

    int init(int numSensors, int * addrs, _themotype * types) { // assume that numSensors is < max Size of packet. Add some error checking here
      _addrs = (int *)malloc(numSensors);
      _cryo_amp_boards = (Adafruit_MCP9600 **)malloc(numSensors * sizeof(Adafruit_MCP9600));

      _numSensors = numSensors;

      for (int i = 0; i < numSensors; i++) {
        _addrs[i] = addrs[i];
        _cryo_amp_boards[i] = new Adafruit_MCP9600();

        if (!_cryo_amp_boards[i]->begin(addrs[i])) {
          Serial.println("Error initializing cryo board at Addr 0x" + String(addrs[i], HEX));
          return -1;
        }

        _cryo_amp_boards[i]->setADCresolution(MCP9600_ADCRESOLUTION_18);
        _cryo_amp_boards[i]->setThermocoupleType(types[i]);
        _cryo_amp_boards[i]->setFilterCoefficient(3);
        _cryo_amp_boards[i]->enable(true);
      }

      return 0;
    }

    void readCryoTemps(float *data) {
      for (int i = 0; i < _numSensors; i++) {
        data[i] = _cryo_amp_boards[i]->readThermocouple();
      }
      data[_numSensors] = -1;
    }

    int freeAllResources() {
        free(_cryo_amp_boards);
        free(_addrs);
        return 0;
    }

  }
}

#endif
