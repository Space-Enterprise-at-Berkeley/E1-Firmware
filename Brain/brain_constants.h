#include <TimeLib.h>
#include <ducer.h>

/*
 * Data structure to store all information relevant to a specific sensor type.
 */
struct sensorInfo {
  String sensor_name;
  int board_address;
  int sensor_id;
  int overall_id;
  int clock_freq;
  void (*dataReadFunc)(float *data);
  // sensorInfo(String n, int b, byte s, int o, int f) : sensor_name(n), board_address(b), sensor_id(s), overall_id(o), clock_freq(f) {}
};

/*
 * Data structure to store all information relevant to a specific valve.
 */
struct valveInfo {
  String name;
  int id;
  int (*openValve)();
  int (*closeValve)();
};

/**************EDIT ONLY THE CONSTANTS BELOW***************/

/*
 * Array of all sensors we would like to get data from.
 */
sensorInfo all_ids[11] = {
  // local sensors
   {"LOX Injector Low Pressure",  -1, -1, 1, 1, &(Ducers::readLOXInjectorPressure)} //example
};

sensorInfo sensor = {"", 0, 0, 0, 0, NULL};

/* 
 *  Stores how often we should be requesting data from each sensor.
 */
int sensor_checks[sizeof(all_ids)/sizeof(sensorInfo)][2];

valveInfo valve = {"",0,0,0};

valveInfo valve_ids[7] = {
  {"LOX 2 Way", 20, &(Solenoids::armLOX), &(Solenoids::disarmLOX)}, //example
};

int numValves = 1;

/***************************/
