/*
  solenoids.h - A c++ library to interface with all of the solenoids on our rocket.
  Created by Vamshi Balanaga, Sept 15, 2020.
*/
#ifndef __LINACT__
#define __LINACT__

#include <Arduino.h>
#include <command.h>

using namespace std;

namespace LinearActuators {

  extern uint8_t _numlinearActuators;
  extern uint8_t _numPairs;

  extern uint8_t * _in1Pins;
  extern uint8_t * _in2Pins;

  extern uint8_t * _in1PinState;
  extern uint8_t * _in2PinState;

  extern uint8_t * _pairIds;
  extern uint8_t * _commandIds;

  void init(uint8_t numActuators, uint8_t numPairs, uint8_t * in1Pins, uint8_t * in2Pins, uint8_t * pairIds, uint8_t * commandIds);
  void getAllStates(float *data);
  void driveForward(uint8_t actuatorId, bool activatePair = true);
  void driveBackward(uint8_t actuatorId, bool activatePair = true);
  void brake(uint8_t actuatorId, bool activatePair = true);
  void switchOff(uint8_t actuatorId, bool activatePair = true);

  class LinActCommand : public Command {

    typedef void (*func_t)();

    public:
      LinActCommand(std::string name, uint8_t id, bool pair, func_t forward, func_t back, func_t off, func_t damp):
        Command(name, id),
        _forward(forward),
        _backward(back),
        _off(off),
        _brake(damp)
      {}

      LinActCommand(std::string name, func_t forward, func_t back, func_t off, func_t damp):
        Command(name),
        _forward(forward),
        _backward(back),
        _off(off),
        _brake(damp)
      {}

      void parseCommand(float *data) {
        #ifdef DEBUG
          Serial.println("lin act, parse command");
          Serial.flush();
        #endif
        // data[1] will specify millis, or full extension
        if (data[0] == 0) { // off
          _off();
        } else if (data[0] == 1)  { // forward
          _forward();
        }  else if (data[0] == 2)  { // backward
          _backward();
        } else if (data[0] == 3)  { // brake
          _brake();
        }
      }

      void confirmation(float *data) {
        LinearActuators::getAllStates(data);
      }

    private:
      func_t _forward;
      func_t _backward;
      func_t _brake;
      func_t _off;
  };

  extern LinActCommand zero;
  extern LinActCommand one;
  extern LinActCommand two;
  extern LinActCommand three;
  extern LinActCommand four;
  extern LinActCommand five;
  extern LinActCommand six;
  extern LinActCommand seven;
}

#endif /* end of include guard: LINACT */
