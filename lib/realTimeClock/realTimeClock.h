/*
  realTimeClock.h - A c++ library to initialize the Teensy 4.1's real-time clock and related functions.
  Created by Salvador Salcedo.
*/

#ifndef __REALTIMECLOCK__
#define __REALTIMECLOCK__

#include <TimeLib.h>
#include <DS1307RTC.h>
#include <stringPatch.h>

using namespace std;
using namespace patch;

namespace RealTimeClock {
    void init() {
      if(timeStatus() != timeSet) 
        setSyncProvider(RTC.get);
    }

    string getFileTime() {
      string result = to_string(month()) + "-";
      result += to_string(day()) + "-";
      result += to_string(year()) + "_T";
      result += to_string(hour()) + ":";
      result += to_string(minute()) + ":";
      result += to_string(second()) + "_" + "+8:00";
      return result;
    }
}

#endif /* end of include guard: REALTIMECLOCK */