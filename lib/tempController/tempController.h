/*
  tempControl.h - A c++ algorithm that maintains the temperature at a given set point.
  Two versions: One very simple, naive algorithm. (Needs to be empirically tested to verify).
  Another, more complex control loop that should reject all disturbances quite well.
  Created by Vamshi Balanaga, Oct 7, 2020.
*/

#ifndef __TEMP_CONTROLLER__
#define __TEMP_CONTROLLER__
#include <cmath>
#include "pid.h"
#include <actuator.h>

using namespace std;


class TempController {
  private:
    int _algorithmChoice; // 1 - naive, 2 - actual control theory
    int _setPointTemp;
    int _heaterPin = 0;
    int _heaterOutput = 0;

    float k_p = 100;
    float k_i = 0.0000001; // low because t is in millis
    float k_d = 1000;

    PID *controller = new PID(255, 0, k_p, k_i, k_d);

  protected:
    bool humanOverride = false;
    uint16_t humanSpecifiedValue = 300;

  public:

    TempController(int tempSetPoint, int algorithmChoice, int heaterPin);
    int calculateOutput(float currTemp);
    float controlTemp(float currTemp);
};


class HeaterActuator : public Actuator, public TempController {

  public:
    HeaterActuator(std::string name, uint8_t id, int tempSetPoint, int algorithmChoice, int heaterPin):
      Actuator(name, id),
      TempController(tempSetPoint, algorithmChoice, heaterPin)
    {}

    HeaterActuator(std::string name, int tempSetPoint, int algorithmChoice, int heaterPin):
      Actuator(name),
      TempController(tempSetPoint, algorithmChoice, heaterPin)
    {}

    void parseCommand(float *data) {
      if (data[0] == 300)
        humanOverride = false;
      else {
        humanOverride = true;
        humanSpecifiedValue = max(0, min(255, data[0]));
      }
    }

    void confirmation(float *data) {
      data[0] = humanOverride;
      data[1] = (humanOverride)? humanSpecifiedValue : -1;
      data[2] = -1;
    }
};
#endif
