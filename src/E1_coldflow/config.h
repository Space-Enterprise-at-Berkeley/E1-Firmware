#include <solenoids.h>
#include <Analog_Thermocouple.h>
#include <Cryo_Thermocouple.h>
#include "common_fw.h"
#include <ADS1219.h>


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
struct sensorInfo **sensors;

const int numValves = 9;
struct valveInfo **valves;

#define LOX_2_PIN 0
#define LOX_5_PIN 2
#define LOX_GEMS_PIN 4

#define PROP_2_PIN 1
#define PROP_5_PIN 3
#define PROP_GEMS_PIN 5

#define HIGH_SOL_PIN 6

const float batteryMonitorShuntR = 0.002; // ohms
const float batteryMonitorMaxExpectedCurrent = 10; // amps

namespace config {
  void setup() {
    debug("Initializing ADCs", DEBUG);
    // initialize all ADCs
    ads = new ADS1219*[numADCSensors];
    for (int i = 0; i < numADCSensors; i++) {
      ads[i] = new ADS1219(adcDataReadyPins[i], ADSAddrs[i], &Wire);
      ads[i]->setConversionMode(SINGLE_SHOT);
      ads[i]->setVoltageReference(REF_EXTERNAL);
      ads[i]->setGain(ONE);
      ads[i]->setDataRate(1000);
      pinMode(adcDataReadyPins[i], INPUT_PULLUP);
      ads[i]->calibrate();
    }

    debug("Initializing valves", DEBUG);
    valves = new valveInfo*[numValves];
    *valves[0] = {"LOX 2 Way", 20, &(Solenoids::armLOX), &(Solenoids::disarmLOX), &(Solenoids::getAllStates)};
    *valves[1] = {"LOX 5 Way", 21, &(Solenoids::openLOX), &(Solenoids::closeLOX), &(Solenoids::getAllStates)};
    *valves[2] = {"LOX GEMS", 22, &(Solenoids::ventLOXGems), &(Solenoids::closeLOXGems), &(Solenoids::getAllStates)};
    *valves[3] = {"Propane 2 Way", 23, &(Solenoids::armPropane), &(Solenoids::disarmPropane), &(Solenoids::getAllStates)};
    *valves[4] = {"Propane 5 Way", 24, &(Solenoids::openPropane), &(Solenoids::closePropane), &(Solenoids::getAllStates)};
    *valves[5] = {"Propane GEMS", 25, &(Solenoids::ventPropaneGems), &(Solenoids::closePropaneGems), &(Solenoids::getAllStates)};
    *valves[6] = {"High Pressure Solenoid", 26, &(Solenoids::activateHighPressureSolenoid), &(Solenoids::deactivateHighPressureSolenoid), &(Solenoids::getAllStates)};
    *valves[7] = {"Arm Rocket", 27, &(Solenoids::armAll), &(Solenoids::disarmAll), &(Solenoids::getAllStates)};
    *valves[8] = {"Launch Rocket", 28, &(Solenoids::LAUNCH), &(Solenoids::endBurn), &(Solenoids::getAllStates)};

    debug("Initializing sensors", DEBUG);
    sensors = new sensorInfo*[numSensors];
    *sensors[0] = {"Temperature",   FLIGHT_BRAIN_ADDR, 0, 3}; //&(testTempRead)}, //&(Thermocouple::readTemperatureData)},
    *sensors[1] = {"All Pressure",  FLIGHT_BRAIN_ADDR, 1, 1};
    *sensors[2] = {"Battery Stats", FLIGHT_BRAIN_ADDR, 2, 3};
    *sensors[3] = {"Aux temp",      FLIGHT_BRAIN_ADDR, 4, 1};

    pinMode(LOX_2_PIN, OUTPUT);
    pinMode(LOX_5_PIN, OUTPUT);
    pinMode(LOX_GEMS_PIN, OUTPUT);

    pinMode(PROP_2_PIN, OUTPUT);
    pinMode(PROP_5_PIN, OUTPUT);
    pinMode(PROP_GEMS_PIN, OUTPUT);

    pinMode(HIGH_SOL_PIN, OUTPUT);

  }
}
