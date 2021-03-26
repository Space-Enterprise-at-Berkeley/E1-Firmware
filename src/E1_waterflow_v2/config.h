#include <solenoids.h>
#include <Analog_Thermocouple.h>
#include "common_fw.h"
#include <ADS8167.h>
#include <INA219.h>


#define FLIGHT_BRAIN_ADDR 0x00

std::string str_file_name = "E1_waterflow_v2.txt";
const char * file_name = str_file_name.c_str();

const uint8_t numADCSensors = 2;
uint8_t adcCSPins[numADCSensors] = {37, 36};
uint8_t adcDataReadyPins[numADCSensors] = {26, 27};
uint8_t adcAlertPins[numADCSensors] = {9, 10};
ADS8167 ads[numADCSensors];
ADC * adsPointers[numADCSensors];

const int numAnalogThermocouples = 1;
uint8_t thermAdcIndices[numAnalogThermocouples] = {0};
uint8_t thermAdcChannels[numAnalogThermocouples] = {4};

const uint8_t numPressureTransducers = 8;
uint8_t ptAdcIndices[numPressureTransducers] = {0, 0, 0, 0, 1, 1, 1, 1}; //not using 1-0 or 1-3
uint8_t ptAdcChannels[numPressureTransducers] = {0, 1, 2, 3, 4, 5, 6, 7};
uint8_t ptTypes[numPressureTransducers] = {1, 1, 1, 1, 2, 1, 1, 1};

const uint8_t numPowerSupplyMonitors = 2;       //12v  , 8v
uint8_t powSupMonAddrs[numPowerSupplyMonitors] = {0x44, 0x45};
INA219 powerSupplyMonitors[numPowerSupplyMonitors];
INA * powSupMonPointers[numPowerSupplyMonitors];

uint8_t battMonINAAddr = 0x43;

const uint8_t numSensors = 4;
sensorInfo *sensors;

const int numValves = 9;
struct valveInfo *valves;

const uint8_t numSolenoids = 7;   // l2, l5, lg, p2, p5, pg, h
uint8_t solenoidPins[numSolenoids] = {5,  3,  1,  4,  2,  0, 6};

const float batteryMonitorShuntR = 0.002; // ohms
const float batteryMonitorMaxExpectedCurrent = 10; // amps

const float powerSupplyMonitorShuntR = 0.010; // ohms
const float powerSupplyMonitorMaxExpectedCurrent = 5; // amps

namespace config {
  void setup() {
    debug("Initializing ADCs");
    for (int i = 0; i < numADCSensors; i++) {
      ads[i].init(&SPI, adcCSPins[i], adcDataReadyPins[i], adcAlertPins[i]);
      ads[i].setManualMode();
      ads[i].setAllInputsSeparate();
      pinMode(adcDataReadyPins[i], INPUT_PULLUP);
      adsPointers[i] = &ads[i];
      // ads[i]->calibrate();
    }

    debug("Initializing Power Supply monitors");
    for (int i = 0; i < numPowerSupplyMonitors; i++) {
        powerSupplyMonitors[i].begin(&Wire, powSupMonAddrs[i]);
        powerSupplyMonitors[i].configure(INA219_RANGE_16V, INA219_GAIN_40MV, INA219_BUS_RES_12BIT, INA219_SHUNT_RES_12BIT_1S);
        powerSupplyMonitors[i].calibrate(powerSupplyMonitorShuntR, powerSupplyMonitorMaxExpectedCurrent);
        powSupMonPointers[i] = &powerSupplyMonitors[i];
    }

    debug("Initializing sensors");
    sensors = new sensorInfo[numSensors];
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
