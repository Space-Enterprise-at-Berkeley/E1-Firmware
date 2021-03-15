#include <solenoids.h>
#include <actuator.h>
#include <Analog_Thermocouple.h>
#include <Cryo_Thermocouple.h>
#include <tempController.h>
#include "common_fw.h"
#include <ADS1219.h>
#include "Automation.h"

#define FLIGHT_BRAIN_ADDR 0x00
#define DEBUG 1

std::string str_file_name = "E1_coldflow.txt";
const char * file_name = str_file_name.c_str();

const int numCryoTherms = 4;
// therm[2] = lox adapter tree pt, therm[3] = lox adapter tree gems
// ADDR = GND, VDD, 10k & 4.3K, 10K & 13K
int cryoThermAddrs[numCryoTherms] = {0x60, 0x67, 0x62, 0x64};
_themotype cryoTypes[numCryoTherms] = {MCP9600_TYPE_J, MCP9600_TYPE_T, MCP9600_TYPE_T, MCP9600_TYPE_K};
Adafruit_MCP9600 cryo_boards[numCryoTherms];

const int numADCSensors = 2;
int ADSAddrs[numADCSensors] = {0b1001010, 0b1001000};
int adcDataReadyPins[numADCSensors] = {29, 28};
ADS1219 ads[numADCSensors];

const int numAnalogThermocouples = 1;
int thermAdcIndices[numAnalogThermocouples] = {1};
int thermAdcChannels[numAnalogThermocouples] = {2};

const int numPressureTransducers = 7;
int ptAdcIndices[numPressureTransducers] = {0, 0, 0, 0, 1, 1, 1}; //not using 1-0 or 1-3
int ptAdcChannels[numPressureTransducers] = {0, 1, 2, 3, 2, 1, 3};
int ptTypes[numPressureTransducers] = {1, 1, 1, 1, 2, 1, 1};

const uint8_t numSensors = 6;
sensorInfo *sensors;

const uint8_t numActuators = 13;
Actuator *backingStore[numActuators];
ActuatorArray actuators(numActuators, backingStore);

const uint8_t numSolenoids = 7;   // l2, l5, lg, p2, p5, pg, h
uint8_t solenoidPins[numSolenoids] = {0,  2,  4,  1,  3,  5, 6};
const uint8_t numSolenoidActuators = 9;    //       l2, l5, lg, p2, p5, pg,  h, arm, launch
uint8_t solenoidActuatorIds[numSolenoidActuators] = {20, 21, 22, 23, 24, 25, 26,  27, 28};

const uint8_t loxAdapterPTHeaterPin = 9;
const uint8_t loxGemsHeaterPin = 7;

const float batteryMonitorShuntR = 0.002; // ohms
const float batteryMonitorMaxExpectedCurrent = 10; // amps

HeaterActuator loxPTHeater("LOX PT Heater", 40, 10, 2, loxAdapterPTHeaterPin); // setPoint = 10 C, alg = PID
HeaterActuator loxGemsHeater("LOX Gems Heater", 41, 10, 2, loxGemsHeaterPin); // setPoint = 10C, alg = PID

AutomationSequenceActuator fullFlow("Perform Flow", 29, &(Automation::beginBothFlow), &(Automation::endBothFlow));
AutomationSequenceActuator loxFlow("Perform LOX Flow", 30, &(Automation::beginLoxFlow), &(Automation::endLoxFlow));

namespace config {
  void setup() {

    debug("Initializing ADCs");
    for (int i = 0; i < numADCSensors; i++) {
      ads[i].init(adcDataReadyPins[i], ADSAddrs[i], &Wire);
      ads[i].setConversionMode(SINGLE_SHOT);
      ads[i].setVoltageReference(REF_EXTERNAL);
      ads[i].setGain(ONE);
      ads[i].setDataRate(1000);
      pinMode(adcDataReadyPins[i], INPUT_PULLUP);
      // ads[i]->calibrate();
    }

    debug("Initializing sensors");
    sensors = new sensorInfo[numSensors];
    // the ordering in this array defines order of operation, not id
    sensors[0] = {"All Pressure",  FLIGHT_BRAIN_ADDR, 1, 1};
    sensors[1] = {"Battery Stats", FLIGHT_BRAIN_ADDR, 2, 3};
    sensors[2] = {"Cryo Temps",      FLIGHT_BRAIN_ADDR, 4, 3};
    sensors[3] = {"Lox PT Temperature",   FLIGHT_BRAIN_ADDR, 0, 4}; //&(testTempRead)}, //&(Thermocouple::readTemperatureData)},
    sensors[4] = {"Number Packets Sent", FLIGHT_BRAIN_ADDR, 5, 10};
    sensors[5] = {"LOX Gems Temp", FLIGHT_BRAIN_ADDR, 6, 4};

    debug("Initializing actuators");
    actuators.insert(&Solenoids::lox_2);
    actuators.insert(&Solenoids::lox_5);
    actuators.insert(&Solenoids::lox_G);
    actuators.insert(&Solenoids::prop_2);
    actuators.insert(&Solenoids::prop_5);
    actuators.insert(&Solenoids::prop_G);
    actuators.insert(&Solenoids::high_p);
    actuators.insert(&Solenoids::arm_rocket);
    actuators.insert(&Solenoids::launch);
    actuators.insert(&fullFlow);
    actuators.insert(&loxFlow);
    actuators.insert(&loxPTHeater);
    actuators.insert(&loxGemsHeater);

    pinMode(loxAdapterPTHeaterPin, OUTPUT);
    pinMode(loxGemsHeaterPin, OUTPUT);
  }
}
