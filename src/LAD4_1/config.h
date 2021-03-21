
#include "common_fw.h"



// ============ THIS IS NOT UP TO DATE ==============
// ============ THIS NEEDS TO BE SPECIFIED ==========


#define FLIGHT_BRAIN_ADDR 0x00
#define DEBUG 0

std::string str_file_name = "LAD4_1.txt";
const char * file_name = str_file_name.c_str();

const uint8_t numSensors = 7;
sensorInfo *sensors;

const float batteryMonitorShuntR = 0.002; // ohms
const float batteryMonitorMaxExpectedCurrent = 10; // amps



namespace config {
  void setup() {

    debug("Initializing sensors", DEBUG);
    sensors = new sensorInfo[numSensors];
    // the ordering in this array defines order of operation, not id
    sensors[0] = {"IMU Acceleration",   FLIGHT_BRAIN_ADDR, 14, 1};
    sensors[1] = {"IMU Orientation", FLIGHT_BRAIN_ADDR, 15, 1};
    sensors[2] = {"Barometer", FLIGHT_BRAIN_ADDR, 13, 1};
    sensors[3] = {"Battery Stats", FLIGHT_BRAIN_ADDR, 2, 3};
    sensors[4] = {"GPS Lat Long",  FLIGHT_BRAIN_ADDR, 11, 3};
    sensors[5] = {"GPS AUX",      FLIGHT_BRAIN_ADDR, 12, 3};
    sensors[6] = {"Number Packets Sent", FLIGHT_BRAIN_ADDR, 5, 10};

  }
}
