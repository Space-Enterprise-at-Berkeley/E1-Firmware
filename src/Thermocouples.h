#pragma once

#include <Arduino.h>
#include <Scheduler.h>
#include <Adafruit_MCP9600.h>

namespace Thermocouples
{

  class Thermocouples: public Task
  {
  public:
    //taken from old code:
    int init(Adafruit_MCP9600 *cryo_boards, uint8_t * addrs, _themotype * types, TwoWire *theWire, float *latestReads);
    //taken straight from scheduler.h:
    void run();
    uint32_t timestamp;
    uint32_t value;
  private:
    Adafruit_MCP9600 * cryo_amp_board;
  };


}
