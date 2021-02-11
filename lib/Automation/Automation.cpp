/*
  Automation.h - A c++ library including automated sequences triggered by
  commands or particular events (e.g. end of flow, overpressurization)
*/

#include "Automation.h"

using namespace std;

namespace Automation {

//-----------------------Variables-----------------------

  bool _startup = false;
  int _startupPhase = 0;
  uint32_t _startupTimer;

  bool _flowing = false;

  bool _shutdown = false;
  int _shutdownPhase = 0;
  uint32_t _shutdownTimer;


  /* Delays during startup sequence:
    1 - Between open pressure and open LOX Main
    2 - Between open LOX Main and open Prop Main
    3 - Between open Prop Main and close Main Valve Arm
  */
  int _startupDelays[3] = {1000, 0 ,1000};

  /* Delays during shutdown sequence:
    1 - Between arm 2-way, close high pressure, close Prop and close LOX
    2 - Between close LOX and closing Arming valve & opening Gems vent
  */
  int _shutdownDelays[2] = {0, 750};
  

//-----------------------Functions-----------------------

  
  bool inStartup() {
    return _startup;
  }
  
  bool inFlow() {
    return _flowing;
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
    if (_startupPhase == 0) {
      _startupTimer = millis();
      return true;
    } else {
      uint32_t timeDiff = millis() - _startupTimer;
      if (timeDiff > _startupDelays[_startupPhase - 1]) {
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
    if (_startupPhase == 0) {

          //close both GEMS, open Arming valve, open Pressurant
          Solenoids::closeLOXGems();
          Solenoids::closePropaneGems();
          Solenoids::armLOX();
          Solenoids::activateHighPressureSolenoid();

          Solenoids::getAllStates(data);
          _startupPhase++;

    } else if (_startupPhase == 1) {
      
        // after a delay open LOX main valve
        Solenoids::openLOX();
        Solenoids::openPropane();
        Solenoids::getAllStates(data);
        _startupPhase++;

    } else if (_startupPhase == 2) {
      
        // after a delay open Prop main valve
        // Solenoids::openPropane();
        Solenoids::getAllStates(data);
        _startupPhase++;

    } else if (_startupPhase == 3) {
      
        // after a delay close Arming valve
        Solenoids::disarmLOX();
        Solenoids::getAllStates(data);


        _startupPhase = 0;
        _startup = false;

    }
    else {
      return false;
    }
    _startupTimer = millis();
    return true;
  }


  void shutdownConfirmation(float *data) {
    data[0] = _shutdown ? 1 : 0;
    data[1] = -1;
  }

  bool checkShutdownProgress() {
    if (_shutdownPhase == 0) {
      _shutdownTimer = millis();
      return true;
    } else {
      uint32_t timeDiff = millis() - _shutdownTimer;
      if (timeDiff > _shutdownDelays[_shutdownPhase - 1]) {
        return true;
      } else {
        return false;
      }
    }
  }

  bool advanceShutdown(float *data) {
    if (_shutdownPhase == 0) {
      Solenoids::deactivateHighPressureSolenoid();
      Solenoids::armLOX();
      Solenoids::closePropane();
      Solenoids::getAllStates(data);
      _shutdownPhase++;

    } else if (_shutdownPhase == 1) {
      Solenoids::closeLOX();
      Solenoids::getAllStates(data);
      _shutdownPhase++;

    } else if (_shutdownPhase == 2) {
      Solenoids::disarmLOX();
      Solenoids::ventLOXGems();
      Solenoids::ventPropaneGems();
      Solenoids::getAllStates(data);

      _shutdownPhase = 0;
      _shutdown = false;
      return true;

    } else {
      return false;
    }
    _shutdownTimer = millis();
    return true;
  }
  
} //Automation