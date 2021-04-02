/*
  solenoids.h - A c++ library to interface with all of the solenoids on our rocket.
  Created by Vamshi Balanaga, Sept 15, 2020.
*/
#ifndef __RECOVERY__
#define __RECOVERY__

using namespace std;

namespace Recovery {

  uint8_t drogue_pin, main_pin;

  int drogue_chute_state = 0;
  int main_chute_state = 0;

  int deployedDrogue = 0;
  int deployedMain = 0;

  void init(uint8_t droguePin, uint8_t mainPin) {
    drogue_pin = droguePin;
    main_pin = mainPin;

    drogue_chute_state = 0;
    main_chute_state = 0;

    digitalWrite(drogue_pin, drogue_chute_state);
    digitalWrite(main_pin, main_chute_state);
  }

  void getAllStates(float *data){
    data[0] = deployedDrogue;
    data[1] = deployedMain;
    data[2] = -1;
  }

  bool drogueReleased() {
    return deployedDrogue;
  }

  bool mainReleased() {
    return deployedMain;
  }

  int toggleDrogueChuteActuator() {
    if(drogue_chute_state == 0){
      drogue_chute_state = 1;
    } else {
      drogue_chute_state = 0;
    }
    digitalWrite(drogue_pin, drogue_chute_state);
    return drogue_chute_state;
  }

  int toggleMainChuteActuator() {
    if (main_chute_state == 0){
      main_chute_state = 1;
    } else {
      main_chute_state = 0;
    }
    digitalWrite(main_pin, main_chute_state);
    return main_chute_state;
  }

  int releaseDrogueChute() {
    deployedDrogue = 1;
    if(drogue_chute_state == 0){
      toggleDrogueChuteActuator();
    } else {
      // already active, do nothing.
    }
    return drogue_chute_state;
  }

  int closeDrogueActuator() {
    if(drogue_chute_state == 1) {
      toggleDrogueChuteActuator();
    } else {
      // already closed, do nothing.
    }
    return drogue_chute_state;
  }

  int releaseMainChute() {
    deployedMain = 1;
    if(main_chute_state == 0) {
      toggleMainChuteActuator();
    } else {
      // already active, do nothing.
    }
    return main_chute_state;
  }

  int closeMainActuator() {
    if(main_chute_state == 1){
      toggleMainChuteActuator();
    } else {
      // already closed, do nothing.
    }
    return main_chute_state;
  }
}


#endif /* end of include guard: RECOVERY */
