/*
  tempControl.cpp - A c++ algorithm that maintains the temperature at a given set point.
  Two versions: One very simple, naive algorithm. (Needs to be empirically tested to verify).
  Another, more complex control loop that should reject all disturbances quite well.
  Created by Vamshi Balanaga, Oct 7, 2020.
*/
#include "tempController.h"

TempController::TempController(int tempSetPoint, int algorithmChoice, int heaterPin):
  _heaterPin(heaterPin),
  _algorithmChoice(algorithmChoice),
  _setPointTemp(tempSetPoint)
 {
  if (algorithmChoice > 2 || algorithmChoice < 0) {
    exit(1);
  }
  pinMode(_heaterPin, OUTPUT);
  _expander = nullptr;
  _channel = -1;
}

TempController::TempController(int tempSetPoint, int algorithmChoice, GpioExpander * expander, int8_t channel):
  _algorithmChoice(algorithmChoice),
  _setPointTemp(tempSetPoint),
  _expander(expander),
  _channel(channel)
 {
  if (algorithmChoice > 2 || algorithmChoice < 0) {
    exit(1);
  }
  _heaterPin = -1;
}

int TempController::calculateOutput(float currTemp) {
  int voltageOut = 0;
  if (_algorithmChoice == 1) { // naive
    voltageOut = (int)(k_p * (_setPointTemp - currTemp));
    return max(0, min(255, voltageOut));
  } else if (_algorithmChoice == 2) { // linear control theory solution
    return controller->calculate(_setPointTemp, currTemp);
  } else {
    return -1;
  }
}


float TempController::controlTemp(float currTemp) {
  _heaterOutput = (humanOverride) ? humanSpecifiedValue : calculateOutput(currTemp);
  if(_heaterPin != -1) {
    //analogWrite(_heaterPin, _heaterOutput);
    return _heaterOutput / 255.0;
  } else if (_channel != -1) {
    Serial.println("gpioexpander");
    if (_heaterOutput / 255.0 > 0.5) {
      _expander->turnOn(_channel);
      return 1.0;
    } else {
      _expander->turnOff(_channel);
      return 0.0;
    }
  }
}
