/*
  Automation.h - A c++ library including automated sequences triggered by
  commands or particular events (e.g. end of flow, overpressurization)
*/



#ifndef __Automation__
#define __Automation__

#include <Arduino.h>
#include <solenoids.h>

using namespace std;

namespace Automation {


//-----------------------Variables-----------------------

  bool _startup = false;
  int startupPhase = 0;
  uint32_t startupTimer;

  bool _flowing = false;

  bool _shutdown = false;
  int shutdownPhase = 0;
  uint32_t shutdownTimer;


  /* Delays during startup sequence:
    1 - Between open pressure and open LOX Main
    2 - Between open LOX Main and open Prop Main
    3 - Between open Prop Main and close Main Valve Arm
  */
  int startupDelays[3] = {1000, 0 ,1000};

  /* Delays during shutdown sequence:
    1 - Between arm 2-way, close high pressure, close Prop and close LOX
    2 - Between close LOX and closing Arming valve & opening Gems vent
  */
  int shutdownDelays[2] = {0, 750};
  

//------------------Function Definitions-----------------


  bool inStartup();

  int beginFlow();
  int endFlow();
  void flowConfirmation(float *data);
  
  bool checkStartupProgress(int startupPhase, int startupTimer);
  bool advanceStartup(float *data);


//-----------------------Functions-----------------------

  
  bool inStartup() {
    return _startup;
  }
  
  bool inFlow() {
    return _startup;
  }
  
  bool inShutdown() {
    return _shutdown;
  }


  // Pretending is a valve action, do -1 to indicate that 
  int beginFlow() {
    /* Check if rocket is in required state for a flow:
      Pressure - Closed
      LOX GEMS & Prop GEMS - Open
      Arming Valve - Closed
      LOX Main Valve & Prop Main Valve - Closed
    */
    //&& Solenoids::getLoxGems() && Solenoids::getPropGems()
    _startup = !Solenoids::getHPS() &&
        !Solenoids::getLox2() && !Solenoids::getLox5() && !Solenoids::getProp5();
    if (_startup) {
      Serial.println("Eureka-1 is in Startup");
    }
    return -1;
  }

  void flowConfirmation(float *data) {
    data[0] = _startup ? 1 : 0;
    data[1] = _shutdown ? 1 : 0;
    data[2] = -1;
  }

  // Pretending is a valve action, do -1 to indicate that 
  int endFlow() {
    _shutdown = true;
    Serial.println("Eureka-1 is in Shutdown");
    return 1;
  }

  bool checkStartupProgress() {
    if (startupPhase == 0) {
      startupTimer = millis();
      return true;
    } else {
      uint32_t timeDiff = millis() - startupTimer;
      if (timeDiff > startupDelays[startupPhase - 1]) {
        return true;
      } else {
        return false;
      }
    }
  }

  /* 
    Progress startup given the current startup phase
  */
  bool advanceStartup(float *data) {
    if (startupPhase == 0) {

          //close both GEMS, open Arming valve, open Pressurant
          Solenoids::closeLOXGems();
          Solenoids::closePropaneGems();
          Solenoids::armLOX();
          Solenoids::activateHighPressureSolenoid();

          Solenoids::getAllStates(data);
          startupPhase++;

    } else if (startupPhase == 1) {
      
        // after a delay open LOX main valve
        Solenoids::openLOX();
        Solenoids::openPropane();
        Solenoids::getAllStates(data);
        startupPhase++;

    } else if (startupPhase == 2) {
      
        // after a delay open Prop main valve
        // Solenoids::openPropane();
        Solenoids::getAllStates(data);
        startupPhase++;

    } else if (startupPhase == 3) {
      
        // after a delay close Arming valve
        Solenoids::disarmLOX();
        Solenoids::getAllStates(data);


        startupPhase = 0;
        _startup = false;

    }
    else {
      return false;
    }
    startupTimer = millis();
    return true;
  }


  void shutdownConfirmation(float *data) {
    data[0] = _shutdown ? 1 : 0;
    data[1] = -1;
  }

  bool checkShutdownProgress() {
    if (shutdownPhase == 0) {
      shutdownTimer = millis();
      return true;
    } else {
      uint32_t timeDiff = millis() - shutdownTimer;
      if (timeDiff > shutdownDelays[shutdownPhase - 1]) {
        return true;
      } else {
        return false;
      }
    }
  }

  bool advanceShutdown(float *data) {
    if (shutdownPhase == 0) {
      Solenoids::deactivateHighPressureSolenoid();
      Solenoids::armLOX();
      Solenoids::closePropane();
      Solenoids::getAllStates(data);
      shutdownPhase++;

    } else if (shutdownPhase == 1) {
      Solenoids::closeLOX();
      Solenoids::getAllStates(data);
      shutdownPhase++;

    } else if (shutdownPhase == 2) {
      Solenoids::disarmLOX();
      Solenoids::ventLOXGems();
      Solenoids::ventPropaneGems();
      Solenoids::getAllStates(data);

      shutdownPhase = 0;
      _shutdown = false;
      return true;

    } else {
      return false;
    }
    shutdownTimer = millis();
    return true;
  }
  
}

#endif
