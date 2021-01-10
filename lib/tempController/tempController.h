/*
  tempControl.cpp - A c++ algorithm that maintains the temperature at a given set point.
  Two versions: One very simple, naive algorithm. (Needs to be empirically tested to verify).
  Another, very complex control loop that should reject all disturbances quite well.
  Created by Vamshi Balanaga, Oct 7, 2020.
*/

#ifndef __TEMP_CONTROLLER__
#define __TEMP_CONTROLLER__
#include <cmath>
#include "pid.h"

using namespace std;

namespace tempController {
  int algorithmChoice; // 1 - naive, 2 - actual control theory
  int setPointTemp;
  int heaterPin = 0;
  int heaterOutput = 0;

  float k_p = 100;
  float k_i = 0.0000001; // low because t is in millis
  float k_d = 1000;

  PID *controller = new PID(255, 0, k_p, k_i, k_d);

  int init(int tempSetPoint, int _algorithmChoice, int _heaterPin) {
    if (_algorithmChoice > 2 || _algorithmChoice < 0) {
      return -1;
    }
    heaterPin = _heaterPin;
    pinMode(heaterPin, OUTPUT);
    algorithmChoice = _algorithmChoice;
    setPointTemp = tempSetPoint;
    return 0;
  }

  int calculateOutput(float currTemp) {
    int voltageOut = 0;
    if (algorithmChoice == 1) { // naive
      voltageOut = (int)(k_p * (setPointTemp - currTemp));
      return max(0, min(255, voltageOut));
      // if(voltageOut > 0){
      //   return 1; // this threshold is dependent on the values of k_p, k_i, k_d;
      // } else {
      //   return 0;
      // }
    } else if (algorithmChoice == 2) { // linear control theory solution
      return controller->calculate(setPointTemp, currTemp);
    } else {
      return -1;
    }
  }

  float controlTemp(float currTemp){
    heaterOutput = calculateOutput(currTemp);
    analogWrite(heaterPin, heaterOutput);
    return ((float)(heaterOutput) / 255) * 24; // heater runs at 24 V
  }
};
#endif
