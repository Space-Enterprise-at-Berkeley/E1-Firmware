/*
  solenoids.h - A c++ library to interface with all of the solenoids on our rocket.
  Created by Vamshi Balanaga, Sept 15, 2020.
*/
#ifndef __ACSOLENOIDS__
#define __ACSOLENOIDS__

#include <Arduino.h>
#include <command.h>

using namespace std;

namespace ACSolenoids {

  extern uint8_t *pins;
  extern uint8_t *states;
  extern uint8_t *ids;

  void init(uint8_t numSolenoids, uint8_t * solenoidPins, uint8_t * commandIds);

  int open(int idx);
  int close(int idx);
  // int open1();
  // int close1();
  // int open2();
  // int close2();
  // int open3();
  // int close3();

  void getAllStates(float *data);

  class ACSolenoidCommand : public Command {

    typedef int (*func_t)();

    public:
      ACSolenoidCommand(std::string name, uint8_t id, func_t openFunc, func_t closeFunc):
        Command(name, id),
        openSolenoid(openFunc),
        closeSolenoid(closeFunc)
      {}

      ACSolenoidCommand(std::string name, func_t openFunc, func_t closeFunc):
        Command(name),
        openSolenoid(openFunc),
        closeSolenoid(closeFunc)
      {}

      void parseCommand(float *data) {
        #ifdef DEBUG
          Serial.println("Solenoid, parse command");
          Serial.flush();
        #endif
        if (data[0] == 1) {
          openSolenoid();
        } else {
          closeSolenoid();
        }
      }

      void confirmation(float *data) {
        ACSolenoids::getAllStates(data);
      }

    private:
      func_t openSolenoid;
      func_t closeSolenoid;
  };

  extern ACSolenoidCommand zero;
  extern ACSolenoidCommand one;
  extern ACSolenoidCommand two;
  extern ACSolenoidCommand three;

}
#endif /* end of include guard: SOLENOIDS */
