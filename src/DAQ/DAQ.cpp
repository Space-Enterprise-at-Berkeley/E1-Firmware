/*
   Main code for DAQ - vamshi
*/

#include "config.h"

#include <ducer.h>
#include <tempController.h>
#include <batteryMonitor.h>
#include <powerSupplyMonitor.h>

#define RFSerial Serial

// within loop state variables

uint8_t val_index = 0;
char command[75]; //input command from GS

/*
    Stores how often we should be requesting data from each sensor.
*/
int sensor_checks[numSensors][2];

valveInfo valve;
sensorInfo *sensor;

long startTime;
String packet;

void sensorReadFunc(int id);

Thermocouple::Cryo _cryoTherms;

void setup() {
  Wire.begin();
  Serial.begin(57600);

  delay(3000);

  debug("Setting up Config");
  config::setup();

  debug("Initializing Sensor Frequencies");

  for (int i = 0; i < numSensors; i++) {
    sensor_checks[i][0] = sensors[i].clock_freq;
    sensor_checks[i][1] = 1;
  }

  debug("Starting SD");

  int res = sd.begin(SdioConfig(FIFO_SDIO));
  if (!res) {
    packet = make_packet(101, true);
    RFSerial.println(packet);
  }

  debug("Opening File");
  file.open(file_name, O_RDWR | O_CREAT);

  debug("Writing Dummy Data");
  sdBuffer = new Queue();

  std::string start = "beginning writing data";
  if(!write_to_SD(start, file_name)) { // if unable to write to SD, send error packet
    packet = make_packet(101, true);
    RFSerial.println(packet);
  }

  debug("Initializing Libraries");

  debug("Initializing battery monitor");
  batteryMonitor::init(&Wire1, batteryMonitorShuntR, batteryMonitorMaxExpectedCurrent, battMonINAAddr);
  debug("Initializing power supply monitors");
  powerSupplyMonitor::init(numPowerSupplyMonitors, powSupMonPointers, powSupMonAddrs, powerSupplyMonitorShuntR, powerSupplyMonitorMaxExpectedCurrent, &Wire1);

  debug("Initializing ducers");
  Ducers::init(numPressureTransducers, ptAdcIndices, ptAdcChannels, ptTypes, adsPointers);

  debug("Initializing Thermocouples");
  Thermocouple::Analog::init(numAnalogThermocouples, thermAdcIndices, thermAdcChannels, adsPointers);

  _cryoTherms = Thermocouple::Cryo();
  _cryoTherms.init(numCryoTherms, _cryo_boards, cryoThermAddrs, cryoTypes, &Wire);

  debug("Initializing Load Cell");
  LoadCell::init(loadcells, numLoadCells, lcSckPins, lcDoutPins, lcCalVals);
}

void loop() {
  // process command
  if (RFSerial.available() > 0) {
    int i = 0;

    while (RFSerial.available()) {
      command[i] = RFSerial.read();
      RFSerial.print(command[i]);
      i++;
    }

    debug(String(command));
    int action = decode_received_packet(String(command), &valve, valves, numValves);
    if (action != -1) {
      take_action(&valve, action);
      packet = make_packet(valve.id, false);
      RFSerial.println(packet);
      write_to_SD(packet.c_str(), file_name);
    }
  }

  /*
     Code for requesting data and relaying back to ground station
  */
  for (int j = 0; j < numSensors; j++) {
    if (sensor_checks[j][0] == sensor_checks[j][1]) {
      sensor_checks[j][1] = 1;
    } else {
      sensor_checks[j][1] += 1;
      continue;
    }
    sensor = &sensors[j];
    sensorReadFunc(sensor->id);
    packet = make_packet(sensor->id, false);
    RFSerial.println(packet);

    write_to_SD(packet.c_str(), file_name);
  }
  delay(10);
}


/**
 *
 */
void sensorReadFunc(int id) {
  switch (id) {
    case 1:
      debug("pressures all");
      Ducers::readAllPressures(farrbconvert.sensorReadings);
      break;
    case 2:
      debug("battery stats");
      batteryMonitor::readAllBatteryStats(farrbconvert.sensorReadings);
      break;
    case 3:
      debug("Load Cells");
      LoadCell::readLoadCells(farrbconvert.sensorReadings);
      break;
    case 4:
      debug("Cryo all");
      _cryoTherms.readCryoTemps(farrbconvert.sensorReadings);
      break;
    case 5:
      readPacketCounter(farrbconvert.sensorReadings);
      break;
    default:
      Serial.println("some other sensor");
      break;
  }
}
