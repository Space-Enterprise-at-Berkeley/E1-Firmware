#include <ACSolenoids.h>
#include <common_fw.h>
#include <Automation.h>
#include <INA219.h>
#include <command.h>
#include <tempController.h>
#include <linearActuators.h>

#define FLIGHT_BRAIN_ADDR 0x00

std::string str_file_name = "Actuator_Controller.txt";
const char * file_name = str_file_name.c_str();

#ifdef ETH
IPAddress ip(10, 0, 0, 21); // dependent on local network
#endif

const uint8_t numPowerSupplyMonitors = 1;       //12v  , 8v
uint8_t powSupMonAddrs[numPowerSupplyMonitors] = {0x41};
INA219 powerSupplyMonitors[numPowerSupplyMonitors];
INA * powSupMonPointers[numPowerSupplyMonitors];

const float powerSupplyMonitorShuntR = 0.010; // ohms
const float powerSupplyMonitorMaxExpectedCurrent = 5; // amps

const uint8_t numSensors = 3;
sensorInfo sensors[numSensors];

const uint8_t numLinActs = 7;
uint8_t in1Pins[numLinActs] = {2, 4, 6, 11, 24, 28, 37};
uint8_t in2Pins[numLinActs] = {3, 5, 7, 12, 25, 29, 36};
uint8_t linActCommandIds[numLinActs] = {14, 15, 16, 17, 18, 19, 20};
uint8_t linActINAAddrs[numLinActs] = {0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A};

#ifdef AC1
// const uint8_t numSolenoids = 2;
// uint8_t solenoidPins[numSolenoids] = {15,  23};
// uint8_t solenoidCommandIds[numSolenoids] = {21, 23};
//
const uint8_t numHeaters = 4;
uint8_t heaterPins[numHeaters] = {14, 15, 22, 23};
uint8_t heaterCommandIds[numHeaters] = {10, 11, 12, 13};
uint8_t heaterINAAddr[numHeaters] = {0x40, 0x41, 0x42, 0x43};

HeaterCommand heater1("heater 1", heaterCommandIds[0], 10, 2, heaterPins[0], &Wire, heaterINAAddr[0], powerSupplyMonitorShuntR, powerSupplyMonitorMaxExpectedCurrent); // setPoint = 10 C, alg = PID
// heater1.initINA219(&Wire, heaterINAAddr[0], powerSupplyMonitorShuntR, powerSupplyMonitorMaxExpectedCurrent);
HeaterCommand heater2("heater 2", heaterCommandIds[1], 10, 2, heaterPins[1], &Wire, heaterINAAddr[0], powerSupplyMonitorShuntR, powerSupplyMonitorMaxExpectedCurrent); // setPoint = 10C, alg = PID
// heater2.initINA219(&Wire, heaterINAAddr[1], powerSupplyMonitorShuntR, powerSupplyMonitorMaxExpectedCurrent);
HeaterCommand heater3("heater 3", heaterCommandIds[2], 10, 2, heaterPins[2], &Wire, heaterINAAddr[0], powerSupplyMonitorShuntR, powerSupplyMonitorMaxExpectedCurrent); // setPoint = 10C, alg = PID
// heater3.initINA219(&Wire, heaterINAAddr[2], powerSupplyMonitorShuntR, powerSupplyMonitorMaxExpectedCurrent);
HeaterCommand heater4("heater 4", heaterCommandIds[3], 10, 2, heaterPins[3], &Wire, heaterINAAddr[0], powerSupplyMonitorShuntR, powerSupplyMonitorMaxExpectedCurrent); // setPoint = 10C, alg = PID
// heater4.initINA219(&Wire, heaterINAAddr[3], powerSupplyMonitorShuntR, powerSupplyMonitorMaxExpectedCurrent);

const uint8_t numLinActPairs = 0;
uint8_t linActPairIds[numLinActs] = {0, 1, 2, 3, 4, 5, 6};
#elif AC2
// const uint8_t numSolenoids = 2;
// uint8_t solenoidPins[numSolenoids] = {15,  23};
// uint8_t solenoidCommandIds[numSolenoids] = {21, 23};
//
const uint8_t numHeaters = 2;
uint8_t heaterPins[numHeaters] = {14, 15};
uint8_t heaterCommandIds[numHeaters] = {20, 21};

HeaterCommand heater1("heater 1", heaterCommandIds[0], 10, 2, heaterPins[0]); // setPoint = 10 C, alg = PID
HeaterCommand heater2("heater 2", heaterCommandIds[1], 10, 2, heaterPins[1]); // setPoint = 10C, alg = PID

const uint8_t numLinActPairs = 2;
uint8_t linActPairIds[numLinActs] = {1, 0, 2, 3, 5, 4, 6};


#elif AC3
// const uint8_t numSolenoids = 2;
// uint8_t solenoidPins[numSolenoids] = {15,  23};
// uint8_t solenoidCommandIds[numSolenoids] = {21, 23};
//
const uint8_t numHeaters = 2;
uint8_t heaterPins[numHeaters] = {14, 15};
uint8_t heaterCommandIds[numHeaters] = {20, 21};

HeaterCommand heater1("heater 1", heaterCommandIds[0], 10, 2, heaterPins[0]); // setPoint = 10 C, alg = PID
HeaterCommand heater2("heater 2", heaterCommandIds[1], 10, 2, heaterPins[1]); // setPoint = 10C, alg = PID

const uint8_t numLinActPairs = 0;
uint8_t linActPairIds[numLinActs] = {0, 1, 2, 3, 4, 5, 6};
#endif

uint8_t battMonINAAddr = 0x40;
const float batteryMonitorShuntR = 0.002; // ohms
const float batteryMonitorMaxExpectedCurrent = 10; // amps

const uint8_t numCommands = 11;
Command *backingStore[numCommands] = {&heater1, &heater2, &heater3, &heater4, //&ACSolenoids::zero,  &ACSolenoids::one,
                                      &LinearActuators::zero, &LinearActuators::one, &LinearActuators::two,
                                      &LinearActuators::three, &LinearActuators::four, &LinearActuators::five,
                                      &LinearActuators::six};
CommandArray commands(numCommands, backingStore);

namespace config {
  void setup() {

    debug("Initializing Power Supply monitors");
    for (int i = 0; i < numPowerSupplyMonitors; i++) {
        powerSupplyMonitors[i].begin(&Wire1, powSupMonAddrs[i]);
        powerSupplyMonitors[i].configure(INA219_RANGE_16V, INA219_GAIN_40MV, INA219_BUS_RES_12BIT, INA219_SHUNT_RES_12BIT_1S);
        powerSupplyMonitors[i].calibrate(powerSupplyMonitorShuntR, powerSupplyMonitorMaxExpectedCurrent);
        powSupMonPointers[i] = &powerSupplyMonitors[i];
    }

    debug("Initializing sensors");
    // the ordering in this array defines order of operation, not id
    sensors[0] = {"Battery Stats", FLIGHT_BRAIN_ADDR, 2, 3};
    sensors[1] = {"Number Packets Sent", FLIGHT_BRAIN_ADDR, 5, 10};
    sensors[2] = {"Mosfet Current Draw", FLIGHT_BRAIN_ADDR, 1, 5};
    sensors[3] = {"LinAct Current Draw", FLIGHT_BRAIN_ADDR, 3, 10};
    sensors[4] = {"LinAct States", FLIGHT_BRAIN_ADDR, 4, 1};

  }
}
