/*
   E1_coldflow.cpp - A c++ program that uses I2C to establish communication between
   the sensors and valves inside the rocket & the ground station. Able to send
   data to the ground station via RF. Can receive and process commands sent from
   ground station.
   Created by Vainavi Viswanath, Aug 21, 2020.
*/

#include "config.h"

#include <ducer.h>
#include <batteryMonitor.h>
#include <powerSupplyMonitor.h>

// within loop state variables

uint8_t val_index = 0;
char command[75]; //input command from GS

/*
    Stores how often we should be requesting data from each sensor.
*/
int sensor_checks[numSensors][2];

sensorInfo *sensor;

long startTime;
String packet;
//
// TempController loxPTHeater(10, 2, loxAdapterPTHeaterPin); // setPoint = 10 C, alg = PID, heaterPin = 7
// TempController loxGemsHeater(10, 2, loxGemsHeaterPin); // setPoint = 10 C, alg = PID

void sensorReadFunc(int id);

void setup() {
  Wire.begin();
  Serial.begin(57600);

  delay(3000);

  #ifdef ETH
  debug("Setup Ethernet");
  setupEthernetComms(mac, ip);
  #endif

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
    Serial.println(packet);
    #ifdef ETH
    sendEthPacket(packet.c_str());
    #endif
  }

  debug("Opening File");
  file.open(file_name, O_RDWR | O_CREAT);

  debug("Writing Dummy Data");
  sdBuffer = new Queue();

  std::string start = "beginning writing data";
  if(!write_to_SD(start, file_name)) { // if unable to write to SD, send error packet
    packet = make_packet(101, true);
    Serial.println(packet);
    #ifdef ETH
    sendEthPacket(packet.c_str());
    #endif
  }

  debug("Initializing Libraries");

  debug("Initializing Solenoids");
  ACSolenoids::init(numSolenoids, solenoidPins, solenoidCommandIds);
  LinearActuators::init(numLinActs, numLinActPairs, in1Pins, in2Pins, linActPairIds, linActCommandIds);
  debug("Initializing battery monitor");
  batteryMonitor::init(&Wire, batteryMonitorShuntR, batteryMonitorMaxExpectedCurrent, battMonINAAddr);
  debug("Initializing power supply monitors");
  powerSupplyMonitor::init(numPowerSupplyMonitors, powSupMonPointers, powSupMonAddrs, powerSupplyMonitorShuntR, powerSupplyMonitorMaxExpectedCurrent, &Wire);

  Automation::init();

  commands.updateIds();
}

void loop() {
  // process command
  #ifdef ETH
  if (Udp.parsePacket()) {
    debug("received udp packet");
    IPAddress remote = Udp.remoteIP();
    for (int i=0; i < 4; i++) {
      Serial.print(remote[i], DEC);
      if (i < 3) {
        Serial.print(".");
      }
    }
    if(Udp.remoteIP() == groundIP) {
      debug("received packet came from groundIP");
      receivedCommand = true;
      Udp.read(command, 75);
      debug(String(command));
    }
  }
  #endif
  if (Serial.available() > 0) {
    int i = 0;

    while (Serial.available()) {
      command[i] = Serial.read();
      Serial.print(command[i]);
      i++;
    }
    receivedCommand = true;
  }

  if(receivedCommand) {
    debug(String(command));
    int8_t id = processCommand(String(command));
    if (id != -1) {
      packet = make_packet(id, false);
      Serial.println(packet);
      #ifdef ETH
      sendEthPacket(packet.c_str());
      #endif
      write_to_SD(packet.c_str(), file_name);
    }
    receivedCommand = false;
  }


  if (Automation::_eventList->length > 0) {
    Serial.print(Automation::_eventList->length);
    Serial.println(" events remain");
    Automation::autoEvent* e = &(Automation::_eventList->events[0]);
    if (millis() - Automation::_eventList->timer > e->duration) {

      e->action();

      //Update valve states after each action
      Solenoids::getAllStates(farrbconvert.sensorReadings);
      packet = make_packet(29, false);
      Serial.println(packet);
      #ifdef ETH
      sendEthPacket(packet.c_str());
      #endif

      Automation::removeEvent();
      //reset timer
      Automation::_eventList->timer = millis();
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
    Serial.println(packet);
    #ifdef ETH
    sendEthPacket(packet.c_str());
    #endif
    write_to_SD(packet.c_str(), file_name);

      // After getting new pressure data, check injector pressures to detect end of flow:
    if (sensor->id==1 && Automation::inFlow()){
      float loxInjector = farrbconvert.sensorReadings[2];
      float propInjector = farrbconvert.sensorReadings[3];

      Automation::detectPeaks(loxInjector, propInjector);
    }
  }
  delay(10);
}


/**
 *
 */
void sensorReadFunc(int id) {
  switch (id) {
    // case 0:
    //   debug("cryo specific read");
    //   _cryoTherms.readSpecificCryoTemp(2, farrbconvert.sensorReadings);
    //   farrbconvert.sensorReadings[1] = loxPTHeater.controlTemp(farrbconvert.sensorReadings[0]);
    //   farrbconvert.sensorReadings[2] = -1;
    //   break;
    // case 1:
    //   debug("pressures all");
    //   Ducers::readAllPressures(farrbconvert.sensorReadings);
    //   break;
    // case 2:
    //   debug("battery stats");
    //   batteryMonitor::readAllBatteryStats(farrbconvert.sensorReadings);
    //   break;
    // case 4:
    //   debug("Cryo all");
    //   _cryoTherms.readCryoTemps(farrbconvert.sensorReadings);
    //   break;
    // case 5:
    //   readPacketCounter(farrbconvert.sensorReadings);
    //   break;
    // case 6:
    //   // this hardcoded 3 is kinda sus.
    //   debug("cryo specific read");
    //   _cryoTherms.readSpecificCryoTemp(3, farrbconvert.sensorReadings);
    //   farrbconvert.sensorReadings[1] = loxGemsHeater.controlTemp(farrbconvert.sensorReadings[0]);
    //   farrbconvert.sensorReadings[2] = -1;
    //   break;
    default:
      Serial.println("some other sensor");
      break;
  }
}
