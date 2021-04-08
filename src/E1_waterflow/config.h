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

const uint8_t numSolenoids = 7;   // l2, l5, lg, p2, p5, pg, h
uint8_t solenoidPins[numSolenoids] = {0,  2,  4,  1,  3,  5, 6};
const uint8_t numSolenoidCommands = 9;    //       l2, l5, lg, p2, p5, pg,  h, arm, launch
uint8_t solenoidCommandIds[numSolenoidCommands] = {20, 21, 22, 23, 24, 25, 26,  27, 28};

const float batteryMonitorShuntR = 0.002; // ohms
const float batteryMonitorMaxExpectedCurrent = 10; // amps

AutomationSequenceCommand fullFlow("Perform Flow", 29, &(Automation::beginBothFlow), &(Automation::endBothFlow));
AutomationSequenceCommand loxFlow("Perform LOX Flow", 30, &(Automation::beginLoxFlow), &(Automation::endLoxFlow));


const uint8_t numCommands = 11;
Command *backingStore[numCommands] = {&Solenoids::lox_2,  &Solenoids::lox_5,  &Solenoids::lox_G,
                                        &Solenoids::prop_2, &Solenoids::prop_5, &Solenoids::prop_G,
                                        &Solenoids::high_p, &Solenoids::arm_rocket, &Solenoids::launch,
                                        &fullFlow, &loxFlow};
CommandArray commands(numCommands, backingStore);

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

    debug("Initializing Commands");
    // Commands.insert(&Solenoids::lox_2);
    // Commands.insert(&Solenoids::lox_5);
    // Commands.insert(&Solenoids::lox_G);
    // Commands.insert(&Solenoids::prop_2);
    // Commands.insert(&Solenoids::prop_5);
    // Commands.insert(&Solenoids::prop_G);
    // Commands.insert(&Solenoids::high_p);
    // Commands.insert(&Solenoids::arm_rocket);
    // Commands.insert(&Solenoids::launch);
    // Commands.insert(&fullFlow);
    // Commands.insert(&loxFlow);
    // valves[10] = {"Perform LOX Flow", 30, &(Automation::beginLoxFlow), &(Automation::endLoxFlow), &(Automation::flowConfirmation)};
  }
}
