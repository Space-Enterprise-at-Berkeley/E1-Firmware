#include <solenoids.h>
#include <Analog_Thermocouple.h>
// #include <Cryo_Thermocouple.h>
#include <Adafruit_MCP9600.h>
#include "common_fw.h"
#include <ADS8167.h>
#include <INA219.h>
#include <GpioExpander.h>
#include <LTC4151.h>
#include <Automation.h>
#include <command.h>
#include <tempController.h>
#include <LTC4151.h>

#define FLIGHT_BRAIN_ADDR 0x00

#define AUTO_SHUTDOWN_START 10

std::string str_file_name = "E1_coldflow_v21.txt";
const char * file_name = str_file_name.c_str();

#ifdef ETH
IPAddress ip(10, 0, 0, 42); // dependent on local network
#endif

// Cryo Thermocouple
const uint8_t numCryoTherms = 4;
// therm[2] = lox adapter tree pt, therm[3] = lox adapter tree gems
// ADDR = GND, VDD, 10k & 4.3K, 10K & 13K
//old: middleLoxTankTC,TopLoxTankTC,middlePropTankTC,TopPropTankTC
uint8_t cryoThermAddrs[numCryoTherms] = {0x60, 0x61, 0x62, 0x63};//{0x60, 0x61, 0x62, 0x63}
_themotype cryoTypes[numCryoTherms] = {MCP9600_TYPE_K, MCP9600_TYPE_K, MCP9600_TYPE_K, MCP9600_TYPE_K};
Adafruit_MCP9600 _cryo_boards[numCryoTherms];
float cryoReadsBackingStore[numCryoTherms];

// ADC
const uint8_t numADCSensors = 2;
uint8_t adcCSPins[numADCSensors] = {37, 36};
uint8_t adcDataReadyPins[numADCSensors] = {26, 27};
uint8_t adcAlertPins[numADCSensors] = {9, 10};
ADS8167 ads[numADCSensors];
ADC * adsPointers[numADCSensors];

// Analog Temperature Sensors
const int numAnalogTempSens = 4;
//lox gems, lox tank pt, lox injector, prop injector, prop tank pt, prop gems
//new: lox injector pt, lox tank pt, fuel tank pt, fuel injector pt
uint8_t tempSensAdcIndices[numAnalogTempSens] = {0, 0, 1, 1};//{0, 0, 1, 1, 1, 1}
uint8_t tempSensAdcChannels[numAnalogTempSens] = {4, 5, 6, 7};//{4, 5, 6, 7, 3, 2}

// Pressure Transducers
const uint8_t numPressureTransducers = 6;
//lox tank, prop tank, lox inj, prop inj, pressurant, lox dome, prop dome, lox gems
uint8_t ptAdcIndices[numPressureTransducers] = {0, 0, 0, 0, 1, 1};
uint8_t ptAdcChannels[numPressureTransducers] = {0, 1, 2, 3, 4, 5};
uint32_t ptTypes[numPressureTransducers] = {1000, 1000, 1000, 1000, 5000, 1000};
const uint8_t pressurantIdx = 4;
const uint8_t loxDomeIdx = 5;
const uint8_t propDomeIdx = 6;

// Power Supply Monitors
const uint8_t numPowerSupplyMonitors = 2;       //12v  , 8v
uint8_t powSupMonAddrs[numPowerSupplyMonitors] = {0x44, 0x45};
INA219 powerSupplyMonitors[numPowerSupplyMonitors];
INA * powSupMonPointers[numPowerSupplyMonitors];

// Battery Monitor
uint8_t battMonINAAddr = 0x43;

// GPIO Expander
const uint8_t numGPIOExpanders = 1;
uint8_t gpioExpAddr[numGPIOExpanders] = {TCA6408A_ADDR1};
uint8_t gpioExpIntPin[numGPIOExpanders] = {-1};
GpioExpander heaterCtl(gpioExpAddr[0], gpioExpIntPin[0], &Wire);

// Heaters
const uint8_t numHeaters = 6;
//

// uint8_t heaterChannels[numHeaters] = {2, 0, 5, 1};
// uint8_t heaterCommandIds[numHeaters] = {40, 42, 44, 45};
// uint8_t heaterINAAddrs[numHeaters] = {0x4B, 0x49, 0x4D, 0x4A};
uint8_t heaterChannels[numHeaters] = {2, 3, 1, 0, 4, 5};
uint8_t heaterCommandIds[numHeaters] = {40, 41, 45, 42, 43, 44};
uint8_t heaterINAAddrs[numHeaters] = {0x4B, 0x4C, 0x4A, 0x49, 0x4D, 0x4E};
//removed 0x4E,
// uint8_t heaterINAAddr[numHeaters] = {0x42, 0x43};

HeaterCommand loxTankPTHeater("loxTankPTHeater", heaterCommandIds[0], 10, 2, &heaterCtl, heaterChannels[0], &Wire1, heaterINAAddrs[0], 0.033, 5.0);
HeaterCommand loxGemsHeater("loxGemsHeater", heaterCommandIds[1], 10, 2, &heaterCtl, heaterChannels[1], &Wire1, heaterINAAddrs[1], 0.033, 5.0);
HeaterCommand propTankPTHeater("propTankPTHeater", heaterCommandIds[3], 10, 2, &heaterCtl, heaterChannels[3], &Wire1, heaterINAAddrs[3], 0.033, 5.0);
HeaterCommand propGemsHeater("propGemsHeater", heaterCommandIds[4], 10, 2, &heaterCtl, heaterChannels[4], &Wire1, heaterINAAddrs[4], 0.033, 5.0);
//4
HeaterCommand loxInjectorPTHeater("loxInjectorPTHeater", heaterCommandIds[5], 10, 2, &heaterCtl, heaterChannels[5], &Wire1, heaterINAAddrs[5], 0.033, 5.0);
HeaterCommand propInjectorPTHeater("propInjectorPTHeater", heaterCommandIds[2], 10, 2, &heaterCtl, heaterChannels[2], &Wire1, heaterINAAddrs[2], 0.033, 5.0);

const uint8_t numSensors = 13;
sensorInfo sensors[numSensors];

// Solenoids
const uint8_t numSolenoids = 8;   // l2, l5, lg, p2, p5, pg, h, h enable
uint8_t solenoidPins[numSolenoids] = {5,  3,  1,  4,  2,  0, 6, 39};
const uint8_t numSolenoidCommands = 10;    //       l2, l5, lg, p2, p5, pg,  h, arm, launch , h enable
uint8_t solenoidCommandIds[numSolenoidCommands] = {20, 21, 22, 23, 24, 25, 26,  27, 28     , 31};
uint8_t solenoidINAAddrs[numSolenoids] = {0x40, 0x42, 0x44, 0x41, 0x43, 0x45};

LTC4151 pressurantSolenoidMonitor;
float pressurantSolMonShuntR = 0.02;

const float batteryMonitorShuntR = 0.002; // ohms
const float batteryMonitorMaxExpectedCurrent = 10; // amps

const float powerSupplyMonitorShuntR = 0.010; // ohms
const float powerSupplyMonitorMaxExpectedCurrent = 5; // amps

const float actuatorMonitorShuntR = 0.033; // ohms

AutomationSequenceCommand fullFlow("Perform Flow", 29, &(Automation::beginBothFlow), &(Automation::endBothFlow));
AutomationSequenceCommand loxFlow("Perform LOX Flow", 30, &(Automation::beginLoxFlow), &(Automation::endLoxFlow));

const uint8_t numCommands = 18;
Command *backingStore[numCommands] = {&Solenoids::lox_2,  &Solenoids::lox_5,  &Solenoids::lox_G,
                                        &Solenoids::prop_2, &Solenoids::prop_5, &Solenoids::prop_G,
                                        &Solenoids::high_p, &Solenoids::high_p_enable, &Solenoids::arm_rocket, &Solenoids::launch,
                                        &fullFlow, &loxFlow, &loxTankPTHeater, &loxGemsHeater, &loxInjectorPTHeater, &propTankPTHeater,
                                        &propGemsHeater, &propInjectorPTHeater};
CommandArray commands(numCommands, backingStore);

// Automation
Automation::autoEvent autoEvents[13];
const int burnTime = 30*1000;

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

    heaterCtl.init();

    debug("Initializing Power Supply monitors");
    for (int i = 0; i < numPowerSupplyMonitors; i++) {
        powerSupplyMonitors[i].begin(&Wire, powSupMonAddrs[i]);
        powerSupplyMonitors[i].configure(INA219_RANGE_32V, INA219_GAIN_40MV, INA219_BUS_RES_12BIT, INA219_SHUNT_RES_12BIT_1S);
        powerSupplyMonitors[i].calibrate(powerSupplyMonitorShuntR, powerSupplyMonitorMaxExpectedCurrent);
        powSupMonPointers[i] = &powerSupplyMonitors[i];
    }

    debug("initializing cryo therms");
    for (int i = 0; i < numCryoTherms; i++) {

      if (!_cryo_boards[i].begin(cryoThermAddrs[i], &Wire)) {
        Serial.println("Error initializing cryo board at Addr 0x" + String(cryoThermAddrs[i], HEX));
        return -1;
      }

      _cryo_boards[i].setADCresolution(MCP9600_ADCRESOLUTION_16);
      _cryo_boards[i].setThermocoupleType(cryoTypes[i]);
      _cryo_boards[i].setFilterCoefficient(3);
      _cryo_boards[i].enable(true);
    }

    pressurantSolenoidMonitor.init(LTC4151::F, LTC4151::F, &Wire1);

    debug("Initializing sensors");
    sensors[0] = {"All Pressure",  FLIGHT_BRAIN_ADDR, 1, 1};
    sensors[1] = {"Battery Stats", FLIGHT_BRAIN_ADDR, 2, 50};
    sensors[2] = {"Number Packets Sent", FLIGHT_BRAIN_ADDR, 5, 250};
    sensors[3] = {"Expected Static Pressure", FLIGHT_BRAIN_ADDR, 17, 99};
    sensors[4] = {"Cryo Temps",      FLIGHT_BRAIN_ADDR, 4, 104};
    sensors[5] = {"Lox PT/FTG Temperature",   FLIGHT_BRAIN_ADDR, 0, 96};
    sensors[6] = {"LOX Gems Temp", FLIGHT_BRAIN_ADDR, 6, 97};
    sensors[7] = {"LOX Injector Temp", FLIGHT_BRAIN_ADDR, 19, 103};
    sensors[8] = {"Prop PT/FTG Temp", FLIGHT_BRAIN_ADDR, 16, 102};
    sensors[9] = {"Prop Gems Temp", FLIGHT_BRAIN_ADDR, 8, 101};
    sensors[10] = {"Prop Injector Temp", FLIGHT_BRAIN_ADDR, 60, 100};
    sensors[11] = {"Solenoid currents", FLIGHT_BRAIN_ADDR, 21, 99};
    sensors[12] = {"Solenoid Volages", FLIGHT_BRAIN_ADDR, 22, 98};


  
    // Automation Sequences
    debug("Initializing Ignition Sequence");
    autoEvents[0] = {0, &(Automation::act_closeGems), false};
    autoEvents[1] = {2300, &(Automation::act_pressurizeTanks), false};
    autoEvents[2] = {1000, &(Solenoids::armAll), false}; // igniter
    autoEvents[3] = {2000, &(Automation::act_openLox), false}; //checks for igniter current, if enabled. 
    autoEvents[4] = {0, &(Solenoids::openPropane), false}; // T-0
    autoEvents[5] = {750, &(Automation::state_setFlowing), false};
    autoEvents[6] = {burnTime - 750, &(Solenoids::closePropane), false};
    autoEvents[7] = {0, &(Automation::state_setShutdown), false};
    autoEvents[8] = {200, &(Solenoids::closeLOX), false};
    autoEvents[9] = {650, &(Automation::act_depressurize), false};
    autoEvents[10] = {0, &(Automation::state_setFlowOver), false};
    


    debug("Initializing Shutdown Sequence");
    autoEvents[10] = {0, &(Automation::act_armCloseProp), false};
    autoEvents[11] = {200, &(Solenoids::closeLOX), false};
    autoEvents[12] = {0, &(Automation::act_depressurize), false};
    autoEvents[13] = {650, &(Solenoids::disarmLOX), false};
    autoEvents[14] = {0, &(Automation::state_setFlowOver), false};


    // autoEvents[5] = {300, &(Automation::state_setFlowing), false};

    //  autoEvents[0] = {0, &(Automation::act_pressurizeTanks), false};
    // autoEvents[1] = {2000, &(Solenoids::armAll), false};
    // // igniter
    // autoEvents[2] = {2000, &(Automation::act_armOpenLox), false};
    // autoEvents[3] = {2000, &(Automation::act_armOpenProp), false};
    // autoEvents[4] = {2000, &(Solenoids::disarmLOX), false};
    // autoEvents[5] = {2000, &(Automation::state_setFlowing), false};

  }
}
