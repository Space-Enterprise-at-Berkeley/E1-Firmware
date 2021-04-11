/*
  realTimeClock.h - A c++ library to initialize the Teensy 4.1's real-time clock and related functions.
  Created by Salvador Salcedo.
*/

#ifndef __REALTIMECLOCK__
#define __REALTIMECLOCK__

#include <TimeLib.h>
#include <DS1307RTC.h>
#include <Arduino.h>

using namespace std;

namespace RealTimeClock {

    void init() {
      if(timeStatus() != timeSet) 
        setSyncProvider(RTC.get);
    }

    String getFileTime() {
      String result = String(month()).concat("-");
      result.concat(day());
      result.concat("-");
      result.concat(year());
      result.concat("_T");
      result.concat(hour());
      result.concat(":");
      result.concat(minute());
      result.concat(":");
      result.concat(second());
      result.concat("_");
      result.concat("+8:00");
      return result;
    }
}

#endif /* end of include guard: RTC */