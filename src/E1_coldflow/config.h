#include <solenoids.h>
#include <Thermocouple.h>
#include <common_fw.h>


#define FLIGHT_BRAIN_ADDR 0x00
#define DEBUG 0

std::string str_file_name = "E1_speed_test_results.txt";
const char * file_name = str_file_name.c_str();

const int numCryoTherms = 2;
int cryoThermAddrs[numCryoTherms] = {0x60, 0x67};
_themotype cryoTypes[numCryoTherms] = {MCP9600_TYPE_J, MCP9600_TYPE_T};

const int numADCSensors = 2;
int ADSAddrs[numADCSensors] = {0b1001010, 0b1001000};
int adcDataReadyPins[numADCSensors] = {29, 28};
ADS1219 ** ads;

const int numAnalogThermocouples = 1;
int thermAdcIndices[numAnalogThermocouples] = {1};
int thermAdcChannels[numAnalogThermocouples] = {2};

const int numPressureTransducers = 5;
int ptAdcIndices[numPressureTransducers] = {0, 0, 0, 0, 1};
int ptAdcChannels[numPressureTransducers] = {0, 1, 2, 3, 0};

const uint8_t numSensors = 4;

/*
   Array of all sensors we would like to get data from.
*/
sensorInfo sensors[numSensors] = {
  {"Temperature",                FLIGHT_BRAIN_ADDR, 0, 3}, //&(testTempRead)}, //&(Thermocouple::readTemperatureData)},
  {"All Pressure",               FLIGHT_BRAIN_ADDR, 1, 1},
  {"Battery Stats",              FLIGHT_BRAIN_ADDR, 2, 3},
  {"Aux temp",                   FLIGHT_BRAIN_ADDR, 4, 1},
};

const int numValves = 10;

valveInfo valves[numValves] = {
  {"LOX 2 Way", 20, &(Solenoids::armLOX), &(Solenoids::disarmLOX), &(Solenoids::getAllStates)},
  {"LOX 5 Way", 21, &(Solenoids::openLOX), &(Solenoids::closeLOX), &(Solenoids::getAllStates)},
  {"LOX GEMS", 22, &(Solenoids::ventLOXGems), &(Solenoids::closeLOXGems), &(Solenoids::getAllStates)},
  {"Propane 2 Way", 23, &(Solenoids::armPropane), &(Solenoids::disarmPropane), &(Solenoids::getAllStates)},
  {"Propane 5 Way", 24, &(Solenoids::openPropane), &(Solenoids::closePropane), &(Solenoids::getAllStates)},
  {"Propane GEMS", 25, &(Solenoids::ventPropaneGems), &(Solenoids::closePropaneGems), &(Solenoids::getAllStates)},
  {"High Pressure Solenoid", 26, &(Solenoids::activateHighPressureSolenoid), &(Solenoids::deactivateHighPressureSolenoid), &(Solenoids::getAllStates)},
  {"Arm Rocket", 27, &(Solenoids::armAll), &(Solenoids::disarmAll), &(Solenoids::getAllStates)},
  {"Launch Rocket", 28, &(Solenoids::LAUNCH), &(Solenoids::endBurn), &(Solenoids::getAllStates)},
};
