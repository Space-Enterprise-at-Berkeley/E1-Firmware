/*
  Automation.cpp - A c++ library including automated sequences triggered by
  commands or particular events (e.g. end of flow, overpressurization)
*/


#ifndef __Automation__
#define __Automation__

#include <Arduino.h>
#include <solenoids.h>


using namespace std;

namespace Automation {

//-----------------------Variables-----------------------

  extern bool _startup;
  extern int _startupPhase;
  extern uint32_t _startupTimer;

  extern bool _flowing;

  extern bool _shutdown;
  extern int _shutdownPhase;
  extern uint32_t _shutdownTimer;


  /* Delays during startup sequence:
    1 - Between open pressure and open LOX Main
    2 - Between open LOX Main and open Prop Main
    3 - Between open Prop Main and close Main Valve Arm
  */
  extern int _startupDelays[3];

  /* Delays during shutdown sequence:
    1 - Between arm 2-way, close high pressure, close Prop and close LOX
    2 - Between close LOX and closing Arming valve & opening Gems vent
  */
  extern int _shutdownDelays[2];


//------------------------Structs------------------------

/*
 * Stores details about an event used to determine what to do & when
 */
struct autoEvent {
  int duration;
  int (*action)();
  bool report;
};


struct autoEventList {
  autoEvent* events;
  int length;
};

extern struct autoEventList* _eventList;


//------------------Function Definitions-----------------

  bool init();

  // Relating to State of the Rocket
  bool inStartup();
  bool inFlow();
  bool inShutdown();

  bool addEvent(autoEvent* e);

  // Actions that can be taken/combined as desired
  int beginBothFlow();
  int endBothFlow();

  int openLox();
  // int closeLox();
  // int beginLoxFlow();
  // int endLoxFlow();

  // int beginPropFlow();
  // int endPropFlow();
  // int openProp();
  // int closeProp();
  
  void flowConfirmation(float *data);
  
  bool checkStartupProgress();
  bool advanceStartup(float *data);

  bool checkShutdownProgress();
  bool advanceShutdown(float *data);

}

#endif
