#include <SPI_Thermocouple.h>
#include <common_fw.h>
#include <command.h>

#define FLIGHT_BRAIN_ADDR 0x00

#ifdef ETH
IPAddress ip(10, 0, 0, 12); // dependent on local network
#endif

const uint8_t numCryoTherms = 4;
// therm[2] = lox adapter tree pt, therm[3] = lox adapter tree gems
// ADDR = GND, VDD, 10k & 4.3K, 10K & 13K
uint8_t cryoThermCS[numCryoTherms] = {21, 20, 19, 18};
uint8_t cryoThermCLK = 13;
uint8_t cryoThermDO = 12; 
Adafruit_MAX31855 _cryo_boards[numCryoTherms];
float cryoReadsBackingStore[numCryoTherms];

const uint8_t numSensors = 2;
sensorInfo sensors[numSensors];

namespace config {
  void setup() {
    debug("Initializing sensors");
    // the ordering in this array defines order of operation, not id
    // sensors[1] = {"Battery Stats", FLIGHT_BRAIN_ADDR, 2, 3};
    sensors[0] = {"Cryo Temps",      FLIGHT_BRAIN_ADDR, 4, 3};
    sensors[1] = {"Number Packets Sent", FLIGHT_BRAIN_ADDR, 5, 10};
    // sensors[6] = {"Power Supply Stats", FLIGHT_BRAIN_ADDR, 6, 3};

  }
}
