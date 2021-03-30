/*
  solenoids.h - A c++ library to interface with all of the solenoids on our rocket.
  Created by Vamshi Balanaga, Sept 15, 2020.
*/

#include "linearActuators.h"

using namespace std;

namespace LinearActuators {

  uint8_t _numlinearActuators;
  uint8_t _numPairs;

  uint8_t * _in1Pins;
  uint8_t * _in2Pins;

  uint8_t * _in1PinState;
  uint8_t * _in2PinState;

  // has the id of the other actuator that is paired w/ the given actuator.
  // single actuators have their own id
  uint8_t * _pairIds;

  void init(uint8_t numActuators, uint8_t numPairs, uint8_t * in1Pins, uint8_t * in2Pins, int8_t * pairIds) {
    if(numPairs > numActuators || numPairs % 2 != 0) {
      Serial.println("Error Initializing Linear numActuators! numPairs should be less than numActuators and numPairs should be an even number.");
      Serial.flush();
      exit(1);
    }

    _numlinearActuators = numActuators;
    _numPairs = numPairs;

    _in1Pins = in1Pins;
    _in2Pins = in2Pins;

    _pairIds = pairIds;

    _in1PinState = (uint8_t *)malloc(_numlinearActuators);
    _in2PinState = (uint8_t *)malloc(_numlinearActuators);

    for (int i = 0; i < _numlinearActuators; i++) {
      pinMode(_in1Pins[i], OUTPUT);
      pinMode(_in2Pins[i], OUTPUT);

      _in1PinState[i] = LOW;
      _in2PinState[i] = LOW;

      digitalWrite(_in1Pins[i], _in1PinState[i]);
      digitalWrite(_in2Pins[i], _in2PinState[i]);
    }
  }

  /** In 1, In 2, state
   *   0  , 0   , 0     OFF
   *   0  , 1   , 1     FORWARD
   *   1  , 0   , 2     BACKWARD
   *   1  , 1   , 3     BRAKE
   *  The In1 state is the msb and the In2 state is the lsb.
   *  combine them to form a number.
   */
  void getAllStates(float *data) {
    #ifdef DEBUG
      Serial.println("Actuators, get all states");
      Serial.flush();
    #endif
    for (int i = 0; i < _numlinearActuators; i++){
      data[i] = 2 * _in1PinState[i] + _in2PinState[i];
    }
    data[_numlinearActuators] = -1;
  }

  void driveForward(uint8_t actuatorId, bool activatePair = true) {
    _in1PinState[actuatorId] = 0;
    _in2PinState[actuatorId] = 1;
    digitalWrite(_in1Pins[actuatorId], _in1PinState[actuatorId]);
    digitalWrite(_in2Pins[actuatorId], _in2PinState[actuatorId]);
    driveForward(_pairIds[actuatorId], false);
  }

  void driveBackward(uint8_t actuatorId, bool activatePair = true) {
    _in1PinState[actuatorId] = 1;
    _in2PinState[actuatorId] = 0;
    digitalWrite(_in1Pins[actuatorId], _in1PinState[actuatorId]);
    digitalWrite(_in2Pins[actuatorId], _in2PinState[actuatorId]);
    driveBackward(_pairIds[actuatorId], false);
  }

  void brake(uint8_t actuatorId, bool activatePair = true) {
    _in1PinState[actuatorId] = 1;
    _in2PinState[actuatorId] = 1;
    digitalWrite(_in1Pins[actuatorId], _in1PinState[actuatorId]);
    digitalWrite(_in2Pins[actuatorId], _in2PinState[actuatorId]);
    brake(_pairIds[actuatorId], false);
  }

  void switchOff(uint8_t actuatorId, bool activatePair = true) {
    _in1PinState[actuatorId] = 1;
    _in2PinState[actuatorId] = 0;
    digitalWrite(_in1Pins[actuatorId], _in1PinState[actuatorId]);
    digitalWrite(_in2Pins[actuatorId], _in2PinState[actuatorId]);
    switchOff(_pairIds[actuatorId], false);
  }



}
