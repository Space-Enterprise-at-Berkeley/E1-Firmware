#include <solenoids.h>
#include <Analog_Thermocouple.h>
#include "common_fw.h"
#include <ADS1219.h>
#include "Automation.h"


#define FLIGHT_BRAIN_ADDR 0x00
#define DEBUG 0

std::string str_file_name = "E1_waterflow.txt";
const char * file_name = str_file_name.c_str();

const int numADCSensors = 2;
int ADSAddrs[numADCSensors] = {0b1001010, 0b1001000};
int adcDataReadyPins[numADCSensors] = {29, 28};
ADS1219 ads[numADCSensors];

const int numAnalogThermocouples = 1;
int thermAdcIndices[numAnalogThermocouples] = {1};
int thermAdcChannels[numAnalogThermocouples] = {2};

const int numPressureTransducers = 5;
int ptAdcIndices[numPressureTransducers] = {0, 0, 0, 0, 1};
int ptAdcChannels[numPressureTransducers] = {0, 1, 2, 3, 0};
int ptTypes[numPressureTransducers] = {1, 1, 1, 1, 2};

const uint8_t numSensors = 4;
sensorInfo *sensors;

const uint8_t numActuators = 11;
Actuator *backingStore[numActuators];
ActuatorArray actuators(numActuators, backingStore);

const uint8_t numSolenoids = 7;   // l2, l5, lg, p2, p5, pg, h
uint8_t solenoidPins[numSolenoids] = {0,  2,  4,  1,  3,  5, 6};
const uint8_t numSolenoidActuators = 9;    //       l2, l5, lg, p2, p5, pg,  h, arm, launch
uint8_t solenoidActuatorIds[numSolenoidActuators] = {20, 21, 22, 23, 24, 25, 26,  27, 28};

const float batteryMonitorShuntR = 0.002; // ohms
const float batteryMonitorMaxExpectedCurrent = 10; // amps

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
    sensors[0] = {"Temperature",   FLIGHT_BRAIN_ADDR, 0, 3}; //&(testTempRead)}, //&(Thermocouple::readTemperatureData)},
    sensors[1] = {"All Pressure",  FLIGHT_BRAIN_ADDR, 1, 1};
    sensors[2] = {"Battery Stats", FLIGHT_BRAIN_ADDR, 2, 3};
    sensors[3] = {"Number Packets Sent", FLIGHT_BRAIN_ADDR, 5, 10};

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
    Automation::fullFlow.setId(29);
    actuators.insert(&Automation::fullFlow);
    // valves[9] = {"Perform Flow", 29, &(Automation::beginBothFlow), &(Automation::endBothFlow), &(Automation::flowConfirmation)};
    Automation::loxFlow.setId(30);
    actuators.insert(&Automation::loxFlow);
    // valves[10] = {"Perform LOX Flow", 30, &(Automation::beginLoxFlow), &(Automation::endLoxFlow), &(Automation::flowConfirmation)};
  }
}
