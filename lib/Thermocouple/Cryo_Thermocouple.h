/*
  Cryo_Thermocouple.h - A c++ library to interface with heavy duty cryo/high temp
  thermocouples using the MCP9600 chip.
  Created by Vamshi Balanaga, Jan 20, 2021.
*/

#ifndef __CRYO_THERMS__
#define __CRYO_THERMS__

#include <Adafruit_MCP9600.h>

namespace Thermocouple {
  class Cryo {
    public:
      int init(int numSensors, int * addrs, _themotype * types); // assume that numSensors is < max Size of packet. Add some error checking here
      void readCryoTemps(float *data);
      void readSpecificCryoTemp(int index, float *data);
      int freeAllResources();
    private:
      Adafruit_MCP9600 ** _cryo_amp_boards;
      int * _addrs;
      int _numSensors;
      float * _latestReads;
  };

}

#endif
