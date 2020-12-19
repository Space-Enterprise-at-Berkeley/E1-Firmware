/*
  solenoids.h - A c++ library to interface with all of the solenoids on our rocket.
  Created by Vamshi Balanaga, Sept 15, 2020.
*/
#ifndef __SOLENOIDS__
#define __SOLENOIDS__

using namespace std;

namespace Solenoids {

  // take this pin values on init, and put them in a config file with brain.
  #define LOX_2_PIN 0
  #define LOX_5_PIN 2
  #define LOX_GEMS_PIN 4

  #define PROP_2_PIN 1
  #define PROP_5_PIN 3
  #define PROP_GEMS_PIN 5

  #define HIGH_SOL_PIN 6

  int high_sol_state = 0;

  int lox2_state = 0;
  int lox5_state = 0;
  int lox_gems_state = 0;

  int prop2_state = 0;
  int prop5_state = 0;
  int prop_gems_state = 0;


  void init() {
    lox2_state = 0;
    lox5_state = 0;
    lox_gems_state = 0;

    prop2_state = 0;
    prop5_state = 0;
    prop_gems_state = 0;

    high_sol_state = 0;

    pinMode(LOX_2_PIN, OUTPUT);
    pinMode(LOX_5_PIN, OUTPUT);
    pinMode(LOX_GEMS_PIN, OUTPUT);

    pinMode(PROP_2_PIN, OUTPUT);
    pinMode(PROP_5_PIN, OUTPUT);
    pinMode(PROP_GEMS_PIN, OUTPUT);

    pinMode(HIGH_SOL_PIN, OUTPUT);

    digitalWrite(LOX_2_PIN, lox2_state);
    digitalWrite(LOX_5_PIN, lox5_state);
    digitalWrite(LOX_GEMS_PIN, lox_gems_state);

    digitalWrite(PROP_2_PIN, prop2_state);
    digitalWrite(PROP_5_PIN, prop5_state);
    digitalWrite(PROP_GEMS_PIN, prop_gems_state);

    digitalWrite(HIGH_SOL_PIN, high_sol_state);
  }

  void getAllStates(float *data){
    data[0] = lox2_state;
    data[1] = prop2_state;
    data[2] = lox5_state;
    data[3] = prop5_state;
    data[4] = lox_gems_state;
    data[5] = prop_gems_state;
    data[6] = high_sol_state;
  }

  bool loxArmed() {
    return lox2_state == 1;
  }

  bool propArmed() {
    return prop2_state == 1;
  }

  int toggleHighPressureSolenoid() {
    if(high_sol_state == 0){
      high_sol_state = 1;
    } else {
      high_sol_state = 0;
    }
    digitalWrite(HIGH_SOL_PIN, high_sol_state);
    return high_sol_state;
  }

  int toggleLOX2Way() {
    if (lox2_state == 0){
      lox2_state = 1;
    } else {
      lox2_state = 0;
    }
    digitalWrite(LOX_2_PIN, lox2_state);
    return lox2_state;
  }

  int toggleLOX5Way() {
    if (lox5_state == 0){
      lox5_state = 1;
    } else {
      lox5_state = 0;
    }
    digitalWrite(LOX_5_PIN, lox5_state);
    return lox5_state;
  }

  int toggleLOXGems() {
    if (lox_gems_state == 0){
      lox_gems_state = 1;
    } else {
      lox_gems_state = 0;
    }
    digitalWrite(LOX_GEMS_PIN, lox_gems_state);
    return lox_gems_state;
  }

  int toggleProp2Way() {
    if (prop2_state == 0){
      prop2_state = 1;
    } else {
      prop2_state = 0;
    }
    digitalWrite(PROP_2_PIN, prop2_state);
    return prop2_state;
  }

  int toggleProp5Way() {
    if (prop5_state == 0){
      prop5_state = 1;
    } else {
      prop5_state = 0;
    }
    digitalWrite(PROP_5_PIN, prop5_state);
    return prop5_state;
  }

  int togglePropGems() {
    if (prop_gems_state == 0){
      prop_gems_state = 1;
    } else {
      prop_gems_state = 0;
    }
    digitalWrite(PROP_GEMS_PIN, prop_gems_state);
    return prop_gems_state;
  }

  int activateHighPressureSolenoid() {
    if(high_sol_state == 0){
      toggleHighPressureSolenoid();
    } else {
      // already active, do nothing.
    }
    return high_sol_state;
  }

  int deactivateHighPressureSolenoid() {
    if(high_sol_state == 1){
      toggleHighPressureSolenoid();
    } else {
      // already closed, do nothing.
    }
    return high_sol_state;
  }

  int ventLOXGems(){
    if (lox_gems_state == 0) {
      toggleLOXGems();
    } else {
      // already active, do nothing.
    }
    return lox_gems_state;
  }

  int closeLOXGems(){
    if (lox_gems_state == 1) {
      toggleLOXGems();
    } else {
      // already closed, do nothing.
    }
    return lox_gems_state;
  }

  int ventPropaneGems(){
    if (prop_gems_state == 0) {
      togglePropGems();
    } else {
      // already active, do nothing.
    }
    return prop_gems_state;
  }

  int closePropaneGems(){
    if (prop_gems_state == 1) {
      togglePropGems();
    } else {
      // already closed, do nothing.
    }
    return prop_gems_state;
  }

  int armLOX() {
    if (lox2_state == 0) {
      toggleLOX2Way();
    } else {
      // already active, do nothing.
    }
    return lox2_state;
  }

  int disarmLOX() {
    if (lox2_state == 1) {
      toggleLOX2Way();
    } else {
      // already closed, do nothing.
    }
    return lox2_state;
  }

  int armPropane() {
    if (prop2_state == 0) {
      toggleProp2Way();
    } else {
      // already active, do nothing.
    }
    return prop2_state;
  }

  int disarmPropane() {
    if (prop2_state == 1) {
      toggleProp2Way();
    } else {
      // already closed, do nothing.
    }
    return prop2_state;
  }

  int armAll() {
    return armLOX() & armPropane();
  }

  int disarmAll(){
    return (!disarmLOX() & !disarmPropane());
  }

  int openLOX() {
    if (lox5_state == 0) {
      toggleLOX5Way();
    } else {
      // already active, do nothing.
    }
    return lox5_state;
  }

  int closeLOX() {
    if (lox5_state == 1) {
      toggleLOX5Way();
    } else {
      // already closed, do nothing.
    }
    return lox5_state;
  }

  int openPropane() {
    if (prop5_state == 0) {
      toggleProp5Way();
    } else {
      // already active, do nothing.
    }
    return prop5_state;
  }

  int closePropane() {
    if(prop5_state == 1){
      toggleProp5Way();
    } else {
      // already closed, do nothing.
    }
    return prop5_state;
  }

  int LAUNCH() {
    openLOX();
    openPropane();
    return 1;
  }

  int endBurn() {
    closeLOX();
    closePropane();
    return 1;
  }
}


#endif /* end of include guard: SOLENOIDS */
