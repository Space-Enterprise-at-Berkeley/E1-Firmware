#pragma once

#include <Arduino.h>
#include <Scheduler.h>
#include <Adafruit_MCP9600.h>

namespace Thermocouples
{

  class Thermocouple: public Task
  {
  public:
    //taken from old code:
    int init(Adafruit_MCP9600 *cryo_board, uint8_t * addrs, _themotype * types, TwoWire *theWire, float *latestReads);
    //implementing run from Task class
    void run();
    uint32_t timestamp;
    uint32_t value;
  private:
    Adafruit_MCP9600 * cryo_amp_board;
    uint8_t * _addrs;
    float * _latestRead;
  };


}
