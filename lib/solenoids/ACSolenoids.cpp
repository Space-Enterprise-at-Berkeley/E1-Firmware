/*
  solenoids.h - A c++ library to interface with all of the solenoids on our rocket.
  Created by Vamshi Balanaga, Sept 15, 2020.
*/

#include "ACSolenoids.h"

namespace ACSolenoids {

  uint8_t _numSolenoids;

  uint8_t *_pins;
  uint8_t *_states;
  uint8_t *_ids;

  void init(uint8_t numSolenoids, uint8_t * pins, uint8_t * commandIds) {

    _numSolenoids = numSolenoids;
    _pins = pins;
    _ids = commandIds;

    _states = (uint8_t *) malloc(numSolenoids * sizeof(uint8_t));

    for (int i = 0; i < _numSolenoids; i++) {
      pinMode(_pins[i], OUTPUT);
      _states[i] = 0;
      digitalWrite(_pins[i], _states[i]);
    }
  }

  void getAllStates(float *data) {
    #ifdef DEBUG
      Serial.println("ACSolenoids, get all states");
      Serial.flush();
    #endif
    for (int i = 0; i < _numSolenoids; i++) {
      data[i] = _states[i];
    }
    data[_numSolenoids] = -1;
  }

  int open(int idx) {
    _states[idx] = 1;
    digitalWrite(_pins[idx], _states[idx]);
    return _states[idx];
  }

  int close(int idx) {
    _states[idx] = 0;
    digitalWrite(_pins[idx], _states[idx]);
    return _states[idx];
  }

  ACSolenoidCommand zero("zero", [&] () { return open(0); }, [&] () { return close(0); });
  ACSolenoidCommand one("one", [&] () { return open(1); }, [&] () { return close(1); });
  ACSolenoidCommand two("two", [&] () { return open(2); }, [&] () { return close(2); });
  ACSolenoidCommand three("three", [&] () { return open(3); }, [&] () { return close(3); });
}
