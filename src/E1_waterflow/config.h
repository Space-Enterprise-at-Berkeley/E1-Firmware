#include <solenoids.h>
#include <Analog_Thermocouple.h>
#include "common_fw.h"
#include <ADS1219.h>

#define FLIGHT_BRAIN_ADDR 0x00

std::string str_file_name = "E1_waterflow.txt";
const char * file_name = str_file_name.c_str();

#ifdef ETH
IPAddress ip(10, 0, 0, 177); // depndent on local network
#endif

const int numADCSensors = 2;
uint8_t ADSAddrs[numADCSensors] = {0b1001010, 0b1001000};
uint8_t adcDataReadyPins[numADCSensors] = {29, 28};
ADS1219 ads[numADCSensors];
ADC * adsPointers[numADCSensors];

const int numAnalogThermocouples = 1;
uint8_t thermAdcIndices[numAnalogThermocouples] = {1};
uint8_t thermAdcChannels[numAnalogThermocouples] = {2};

const int numPressureTransducers = 5;
uint8_t ptAdcIndices[numPressureTransducers] = {0, 0, 0, 0, 1};
uint8_t ptAdcChannels[numPressureTransducers] = {0, 1, 2, 3, 0};
uint8_t ptTypes[numPressureTransducers] = {1, 1, 1, 1, 2};

const uint8_t numSensors = 4;
sensorInfo sensors[numSensors];

const int numValves = 9;
struct valveInfo *valves;

const uint8_t numSolenoids = 7;   // l2, l5, lg, p2, p5, pg, h
uint8_t solenoidPins[numSolenoids] = {0,  2,  4,  1,  3,  5, 6};

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
      adsPointers[i] = &ads[i];
      // ads[i]->calibrate();
    }

    debug("Initializing sensors");
    sensors[0] = {"Temperature",   FLIGHT_BRAIN_ADDR, 0, 3}; //&(testTempRead)}, //&(Thermocouple::readTemperatureData)},
    sensors[1] = {"All Pressure",  FLIGHT_BRAIN_ADDR, 1, 1};
    sensors[2] = {"Battery Stats", FLIGHT_BRAIN_ADDR, 2, 3};
    sensors[3] = {"Number Packets Sent", FLIGHT_BRAIN_ADDR, 5, 10};

    debug("Initializing valves");
    valves = new valveInfo[numValves];
    valves[0] = {"LOX 2 Way", 20, &(Solenoids::armLOX), &(Solenoids::disarmLOX), &(Solenoids::getAllStates)};
    valves[1] = {"LOX 5 Way", 21, &(Solenoids::openLOX), &(Solenoids::closeLOX), &(Solenoids::getAllStates)};
    valves[2] = {"LOX GEMS", 22, &(Solenoids::ventLOXGems), &(Solenoids::closeLOXGems), &(Solenoids::getAllStates)};
    valves[3] = {"Propane 2 Way", 23, &(Solenoids::armPropane), &(Solenoids::disarmPropane), &(Solenoids::getAllStates)};
    valves[4] = {"Propane 5 Way", 24, &(Solenoids::openPropane), &(Solenoids::closePropane), &(Solenoids::getAllStates)};
    valves[5] = {"Propane GEMS", 25, &(Solenoids::ventPropaneGems), &(Solenoids::closePropaneGems), &(Solenoids::getAllStates)};
    valves[6] = {"High Pressure Solenoid", 26, &(Solenoids::activateHighPressureSolenoid), &(Solenoids::deactivateHighPressureSolenoid), &(Solenoids::getAllStates)};
    valves[7] = {"Arm Rocket", 27, &(Solenoids::armAll), &(Solenoids::disarmAll), &(Solenoids::getAllStates)};
    valves[8] = {"Launch Rocket", 28, &(Solenoids::LAUNCH), &(Solenoids::endBurn), &(Solenoids::getAllStates)};
  }
}
