
#include "common_fw.h"
#include <recovery.h>

#define FLIGHT_BRAIN_ADDR 0x00
#define DEBUG 0

const uint8_t DROGUE_PIN = 9;
const uint8_t MAIN_CHUTE_PIN = 10;

std::string str_file_name = "LAD4_1.txt";
const char * file_name = str_file_name.c_str();

const uint8_t numSensors = 8;
sensorInfo *sensors;

const float batteryMonitorShuntR = 0.002; // ohms
const float batteryMonitorMaxExpectedCurrent = 10; // amps

const uint8_t recoverAckId = 10;

double mainChuteDeployLoc = 300; // m above start location.

namespace config {
  void setup() {

    //debug("Initializing sensors", DEBUG);
    sensors = new sensorInfo[numSensors];
    // the ordering in this array defines order of operation, not id
    sensors[0] = {"Barometer", FLIGHT_BRAIN_ADDR, 13, 1};
    sensors[1] = {"IMU Acceleration",   FLIGHT_BRAIN_ADDR, 14, 1};
    sensors[2] = {"IMU Orientation", FLIGHT_BRAIN_ADDR, 15, 1};
    sensors[3] = {"Battery Stats", FLIGHT_BRAIN_ADDR, 2, 3};
    sensors[4] = {"GPS Lat Long",  FLIGHT_BRAIN_ADDR, 11, 20};
    sensors[5] = {"GPS AUX",      FLIGHT_BRAIN_ADDR, 12, 20};
    sensors[6] = {"Number Packets Sent", FLIGHT_BRAIN_ADDR, 5, 10};
    sensors[7] = {"Recovery State", FLIGHT_BRAIN_ADDR, 10, 40};

    pinMode(DROGUE_PIN, OUTPUT);
    pinMode(MAIN_CHUTE_PIN, OUTPUT);
  }
}
