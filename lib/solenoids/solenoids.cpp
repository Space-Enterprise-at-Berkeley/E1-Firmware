/*
  solenoids.h - A c++ library to interface with all of the solenoids on our rocket.
  Created by Vamshi Balanaga, Sept 15, 2020.
*/

#include "solenoids.h"

using namespace std;

namespace Solenoids {

  uint8_t lox_2_pin, lox_5_pin, lox_gems_pin;
  uint8_t prop_2_pin, prop_5_pin, prop_gems_pin;
  uint8_t high_sol_pin;

  uint8_t *_actuatorIds;
  uint8_t _numActuators;

  uint8_t high_sol_state = 0;

  uint8_t lox2_state = 0;
  uint8_t lox5_state = 0;
  uint8_t lox_gems_state = 0;

  uint8_t prop2_state = 0;
  uint8_t prop5_state = 0;
  uint8_t prop_gems_state = 0;


  void init(uint8_t numSolenoids, uint8_t * solenoidPins, uint8_t numActuators, uint8_t * actuatorIds) {
    lox2_state = 0;
    lox5_state = 0;
    lox_gems_state = 0;

    prop2_state = 0;
    prop5_state = 0;
    prop_gems_state = 0;

    high_sol_state = 0;

    lox_2_pin = solenoidPins[0];
    lox_5_pin = solenoidPins[1];
    lox_gems_pin = solenoidPins[2];

    prop_2_pin = solenoidPins[3];
    prop_5_pin = solenoidPins[4];
    prop_gems_pin = solenoidPins[5];

    high_sol_pin = solenoidPins[6];

    _numActuators = numActuators;
    _actuatorIds = actuatorIds;

    pinMode(lox_2_pin, OUTPUT);
    pinMode(lox_5_pin, OUTPUT);
    pinMode(lox_gems_pin, OUTPUT);

    pinMode(prop_2_pin, OUTPUT);
    pinMode(prop_5_pin, OUTPUT);
    pinMode(prop_gems_pin, OUTPUT);

    pinMode(high_sol_pin, OUTPUT);

    digitalWrite(lox_2_pin, lox2_state);
    digitalWrite(lox_5_pin, lox5_state);
    digitalWrite(lox_gems_pin, lox_gems_state);

    digitalWrite(prop_2_pin, prop2_state);
    digitalWrite(prop_5_pin, prop5_state);
    digitalWrite(prop_gems_pin, prop_gems_state);

    digitalWrite(high_sol_pin, high_sol_state);

    lox_2.setId(_actuatorIds[0]);
    lox_5.setId(_actuatorIds[1]);
    lox_G.setId(_actuatorIds[2]);

    prop_2.setId(_actuatorIds[3]);
    prop_5.setId(_actuatorIds[4]);
    prop_G.setId(_actuatorIds[5]);

    high_p.setId(_actuatorIds[6]);

    arm_rocket.setId(_actuatorIds[7]);
    launch.setId(_actuatorIds[8]);


  }

  void getAllStates(float *data) {
    #ifdef DEBUG
      Serial.println("Solenoids, get all states");
      Serial.flush();
    #endif
    data[0] = lox2_state;
    data[1] = prop2_state;
    data[2] = lox5_state;
    data[3] = prop5_state;
    data[4] = lox_gems_state;
    data[5] = prop_gems_state;
    data[6] = high_sol_state;
    data[7] = -1;
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
    digitalWrite(high_sol_pin, high_sol_state);
    return high_sol_state;
  }

  int toggleLOX2Way() {
    if (lox2_state == 0){
      lox2_state = 1;
    } else {
      lox2_state = 0;
    }
    digitalWrite(lox_2_pin, lox2_state);
    return lox2_state;
  }

  int toggleLOX5Way() {
    if (lox5_state == 0){
      lox5_state = 1;
    } else {
      lox5_state = 0;
    }
    digitalWrite(lox_5_pin, lox5_state);
    return lox5_state;
  }

  int toggleLOXGems() {
    if (lox_gems_state == 0){
      lox_gems_state = 1;
    } else {
      lox_gems_state = 0;
    }
    digitalWrite(lox_gems_pin, lox_gems_state);
    return lox_gems_state;
  }

  int toggleProp2Way() {
    if (prop2_state == 0){
      prop2_state = 1;
    } else {
      prop2_state = 0;
    }
    digitalWrite(prop_2_pin, prop2_state);
    return prop2_state;
  }

  int toggleProp5Way() {
    if (prop5_state == 0){
      prop5_state = 1;
    } else {
      prop5_state = 0;
    }
    digitalWrite(prop_5_pin, prop5_state);
    return prop5_state;
  }

  int togglePropGems() {
    if (prop_gems_state == 0){
      prop_gems_state = 1;
    } else {
      prop_gems_state = 0;
    }
    digitalWrite(prop_gems_pin, prop_gems_state);
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

   int getHPS() {
    return high_sol_state;
  }

  int getLox2() {
    return lox2_state;
  }

  int getLox5() {
    return lox5_state;
  }

  int getLoxGems() {
    return lox_gems_state;
  }

  int getProp2() {
    return prop2_state;
  }

  int getProp5() {
    return prop5_state;
  }

  int getPropGems() {
    return prop_gems_state;
  }

  SolenoidActuator lox_2("LOX 2 Way", &armLOX, &disarmLOX);
  SolenoidActuator lox_5("LOX 5 Way", &openLOX, &closeLOX);
  SolenoidActuator lox_G("LOX Gems", &ventLOXGems, &closeLOXGems);
  SolenoidActuator prop_2("Propane 2 Way", &armPropane, &disarmPropane);
  SolenoidActuator prop_5("Propane 5 Way", &openPropane, &closePropane);
  SolenoidActuator prop_G("Propane Gems", &ventPropaneGems, &closePropaneGems);

  SolenoidActuator high_p("High Pressure Solenoid", &activateHighPressureSolenoid, &deactivateHighPressureSolenoid);

  SolenoidActuator arm_rocket("Arm Rocket", &armAll, &disarmAll);
  SolenoidActuator launch("Launch Rocket", &LAUNCH, &endBurn);
}
