#include <solenoids.h>
#include <Analog_Thermocouple.h>
#include <Cryo_Thermocouple.h>
#include <common_fw.h>
#include <ADS1219.h>
#include "Automation.h"

#define FLIGHT_BRAIN_ADDR 0x00

std::string str_file_name = "E1_coldflow.txt";
const char * file_name = str_file_name.c_str();

#ifdef ETH
IPAddress ip(10, 0, 0, 177); // dependent on local network
#endif

const uint8_t numCryoTherms = 4;
// therm[2] = lox adapter tree pt, therm[3] = lox adapter tree gems
// ADDR = GND, VDD, 10k & 4.3K, 10K & 13K
uint8_t cryoThermAddrs[numCryoTherms] = {0x60, 0x67, 0x62, 0x64};
_themotype cryoTypes[numCryoTherms] = {MCP9600_TYPE_J, MCP9600_TYPE_T, MCP9600_TYPE_T, MCP9600_TYPE_K};
Adafruit_MCP9600 _cryo_boards[numCryoTherms];

const uint8_t numADCSensors = 2;
uint8_t ADSAddrs[numADCSensors] = {0b1001010, 0b1001000};
uint8_t adcDataReadyPins[numADCSensors] = {29, 28};
ADS1219 ads[numADCSensors];
ADC *adsPointers[numADCSensors];

const uint8_t numAnalogThermocouples = 1;
uint8_t thermAdcIndices[numAnalogThermocouples] = {1};
uint8_t thermAdcChannels[numAnalogThermocouples] = {2};

const uint8_t numPressureTransducers = 7;
uint8_t ptAdcIndices[numPressureTransducers] = {0, 0, 0, 0, 1, 1, 1}; //not using 1-0 or 1-3
uint8_t ptAdcChannels[numPressureTransducers] = {0, 1, 2, 3, 2, 1, 3};
uint8_t ptTypes[numPressureTransducers] = {1, 1, 1, 1, 2, 1, 1};

const uint8_t numSensors = 6;
sensorInfo sensors[numSensors];

const int numValves = 11;
struct valveInfo *valves;

const uint8_t numSolenoids = 7;   // l2, l5, lg, p2, p5, pg, h
uint8_t solenoidPins[numSolenoids] = {0,  2,  4,  1,  3,  5, 6};

const uint8_t loxAdapterPTHeaterPin = 9;
const uint8_t loxGemsHeaterPin = 7;

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
      // ads[i].calibrate();
    }

    debug("Initializing sensors");
    // the ordering in this array defines order of operation, not id
    sensors[0] = {"All Pressure",  FLIGHT_BRAIN_ADDR, 1, 1};
    sensors[1] = {"Battery Stats", FLIGHT_BRAIN_ADDR, 2, 3};
    sensors[2] = {"Cryo Temps",      FLIGHT_BRAIN_ADDR, 4, 3};
    sensors[3] = {"Lox PT Temperature",   FLIGHT_BRAIN_ADDR, 0, 4}; //&(testTempRead)}, //&(Thermocouple::readTemperatureData)},
    sensors[4] = {"Number Packets Sent", FLIGHT_BRAIN_ADDR, 5, 10};
    sensors[5] = {"LOX Gems Temp", FLIGHT_BRAIN_ADDR, 6, 4};

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
    valves[9] = {"Perform Flow", 29, &(Automation::beginBothFlow), &(Automation::endBothFlow), &(Automation::flowConfirmation)};
    valves[10] = {"Perform LOX Flow", 30, &(Automation::beginLoxFlow), &(Automation::endLoxFlow), &(Automation::flowConfirmation)};

    pinMode(loxAdapterPTHeaterPin, OUTPUT);
    pinMode(loxGemsHeaterPin, OUTPUT);
  }
}
