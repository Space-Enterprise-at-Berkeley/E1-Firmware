/*
  solenoids.h - A c++ library to interface with all of the solenoids on our rocket.
  Created by Vamshi Balanaga, Sept 15, 2020.
*/
#ifndef __LINACT__
#define __LINACT__

#include <Arduino.h>

using namespace std;

namespace Actuators {

  extern uint8_t _numlinearActuators;
  extern uint8_t _numPairs;

  extern uint8_t * _in1Pins;
  extern uint8_t * _in2Pins;

  extern uint8_t * _in1PinState;
  extern uint8_t * _in2PinState;

  extern uint8_t * _pairIds;

  void init(uint8_t numActuators, uint8_t numPairs, uint8_t * in1Pins, uint8_t * in2Pins, int8_t * pairIds);
  void getAllStates(float *data);
  void driveForward(uint8_t actuatorId, bool activatePair = true);
  void driveBackward(uint8_t actuatorId, bool activatePair = true);
  void brake(uint8_t actuatorId, bool activatePair = true);
  void switchOff(uint8_t actuatorId, bool activatePair = true);
}

#endif /* end of include guard: LINACT */
