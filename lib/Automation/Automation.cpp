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

 struct autoEventList* _eventList;

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

  bool init() {
    _eventList = new autoEventList;
    _eventList->maxEvents = 10; //arbitrary max of 10 events right now.
    _eventList->events = new autoEvent[_eventList->maxEvents]; 

    _eventList->length = 0;
    return true;
  }


  bool inStartup() {
    return _startup;
  }
  
  bool inFlow() {
    return _flowing;
  }
  
  bool inShutdown() {
    return _shutdown;
  }


  /* 
   * For now copies the passed into autoEvent into the eventList. Copying requires slightly more
   * overhead, but avoids dealing with allocating & deallocation memory for each event 
   */
  bool addEvent(autoEvent* e) {
    if (_eventList->length < 10) {
      (_eventList->events)[_eventList->length] = *e;
      _eventList->length++;
      Serial.println("eventList len!");
      Serial.println(_eventList->length);
      // if first event is being added then need to restart timer.
      if (_eventList->length == 1) {
        _eventList->timer = millis();
      }
      return true;
    } else {
      return false;
    }
  }

  bool removeEvent() {
    if (_eventList->length > 0) {
      //move events 1 - 9 & move to slots 0 - 8, effectively "popping" first event
      memmove(_eventList->events, _eventList->events + 1, sizeof(autoEvent)*(_eventList->maxEvents - 1));

      Automation::_eventList->length--;
    }
  }


  int act_pressurizeTanks() {
    Solenoids::closeLOXGems();
    Solenoids::closePropaneGems();
    Solenoids::activateHighPressureSolenoid();
  }

  int act_openGems() {
    Solenoids::ventLOXGems();
    Solenoids::ventPropaneGems();
  }

  int act_armOpenLox() {
    Solenoids::armLOX();
    Solenoids::openLOX();
  }

  int act_armOpenProp() {
    Solenoids::armLOX();
    Solenoids::openPropane();
  }

  int act_armOpenBoth() {
    Solenoids::armLOX();
    Solenoids::openLOX();
    Solenoids::openPropane();
  }

  int act_armCloseLox() {
    Solenoids::armLOX();
    Solenoids::closeLOX();
  }

  int act_armCloseProp() {
    Solenoids::armLOX();
    Solenoids::closePropane();
  }

  int act_armCloseBoth() {
    Solenoids::armLOX();
    Solenoids::closeLOX();
    Solenoids::closePropane();
  }

  int beginLoxFlow() {
    autoEvent events[3];
    events[0] = {0, &(act_pressurizeTanks), false};
    events[1] = {1000, &(act_armOpenLox), false};
    events[2] = {750, &(Solenoids::disarmLOX), false};
    for (int i = 0; i < 3; i++) addEvent(&events[i]);
  }

  int endLoxFlow() {
    autoEvent events[4];
    events[0] = {0, &(act_armCloseLox), false};
    events[1] = {0, &(Solenoids::deactivateHighPressureSolenoid), false};
    events[2] = {750, &(Solenoids::disarmLOX), false};
    events[3] = {0, &(act_openGems), false};
    for (int i = 0; i < 4; i++) addEvent(&events[i]);
  }

  int openLox() {
    autoEvent events[2];
    events[0] = {0, &(act_armOpenLox), false};
    events[1] = {750, &(Solenoids::disarmLOX), false};
    for (int i = 0; i < 2; i++) addEvent(&events[i]);
  }

  int closeLox() {
    autoEvent e1 = autoEvent{2000, &(Solenoids::closeLOX), false};
    addEvent(&e1);
    return 1;
  }





  // Pretending is a valve action, do -1 to indicate that 
  int beginBothFlow() {
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

  // Responds to initial command to begin/end flow
  void flowConfirmation(float *data) {
    data[0] = _startup ? 1 : 0;
    data[1] = _shutdown ? 1 : 0;
    data[2] = -1;
  }

  // Pretending is a valve action, do -1 to indicate that 
  int endBothFlow() {
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
          Solenoids::activateHighPressureSolenoid();
          Solenoids::armLOX();

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