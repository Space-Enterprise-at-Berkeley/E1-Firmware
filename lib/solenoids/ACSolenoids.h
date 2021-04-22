/*
  solenoids.h - A c++ library to interface with all of the solenoids on our rocket.
  Created by Vamshi Balanaga, Sept 15, 2020.
*/
#ifndef __ACSOLENOIDS__
#define __ACSOLENOIDS__

#include <Arduino.h>
#include <command.h>
#include <Wire.h>
#include <INA219.h>

using namespace std;

namespace ACSolenoids {

  extern uint8_t *pins;
  extern uint8_t *states;
  extern uint8_t *ids;

  extern TwoWire *_wire;
  extern uint8_t *_outputMonAddrs;

  void init(uint8_t numSolenoids, uint8_t * solenoidPins, uint8_t * commandIds, TwoWire *wire, uint8_t *outputMonAddrs, float shuntR, float maxExCurrent);
  void init(uint8_t numSolenoids, uint8_t * solenoidPins, uint8_t * commandIds);

  int open(int idx);
  int close(int idx);

  void getAllStates(float *data);
  void getAllCurrentDraw(float *data);

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

      void initINA219(TwoWire *wire, uint8_t inaAddr, float shuntR, float maxExpectedCurrent) {
        outputMonitor.begin(wire, inaAddr);
        outputMonitor.configure(INA219_RANGE_16V, INA219_GAIN_40MV, INA219_BUS_RES_12BIT, INA219_SHUNT_RES_12BIT_1S);
        outputMonitor.calibrate(shuntR, maxExpectedCurrent);
      }

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

    INA219 outputMonitor;

    private:
      func_t openSolenoid;
      func_t closeSolenoid;

  };

  extern ACSolenoidCommand zero;
  extern ACSolenoidCommand one;
  extern ACSolenoidCommand two;
  extern ACSolenoidCommand three;

  extern ACSolenoidCommand * _commands[];

}
#endif /* end of include guard: SOLENOIDS */
