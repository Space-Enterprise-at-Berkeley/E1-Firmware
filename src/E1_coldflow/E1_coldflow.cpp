/*
   Brain_I2C.ino - A c++ program that uses I2C to establish communication between
   the sensors and valves inside to the rocket with the ground station. Able to send
   data to the ground station via RF. Can receive and process commands sent from
   ground station.
   Created by Vainavi Viswanath, Aug 21, 2020.
*/
#include <Arduino.h>
#include <Wire.h>

#include "config.h"

#include <common_fw.h>
#include <ducer.h>
#include <tempController.h>
#include <batteryMonitor.h>

#define RFSerial Serial6

// within loop state variables
uint8_t board_address = 0;
uint8_t sensor_id = 0;
uint8_t val_index = 0;
char command[50]; //input command from GS

/*
    Stores how often we should be requesting data from each sensor.
*/
int sensor_checks[numSensors][2];

valveInfo valve;
sensorInfo sensor = {"temp", 23, 23, 23};

long startTime;
String packet;

void sensorReadFunc(int id);

void setup() {
  Wire.begin();
  Serial.begin(57600);
  RFSerial.begin(57600);

  delay(3000);
  Serial.println("Initializing Libraries");

  for (int i = 0; i < numSensors; i++) {
    sensor_checks[i][0] = sensors[i].clock_freq;
    sensor_checks[i][1] = 1;
  }

  int res = sd.begin(SdioConfig(FIFO_SDIO));
  if (!res) {
    packet = make_packet(101, true);
    RFSerial.println(packet);
  }
  Serial.println("Opening File");
  Serial.flush();
  file.open(file_name, O_RDWR | O_CREAT);
  file.close();

  Serial.println("Writing Dummy Data");
  Serial.flush();
  // NEED TO DO THIS BEFORE ANY CALLS TO write_to_SD
  sdBuffer = new Queue();

  std::string start = "beginning writing data";
  if(!write_to_SD(start, file_name)) { // if unable to write to SD, send error packet
    packet = make_packet(101, true);
    RFSerial.println(packet);
  }

  Serial.println("Initializing ADCs");
  Serial.flush();
  // initialize all ADCs
  ads = new ADS1219*[numADCSensors];
  for (int i = 0; i < numADCSensors; i++) {
    ads[i] = new ADS1219(adcDataReadyPins[i], ADSAddrs[i], &Wire);
    ads[i]->setConversionMode(SINGLE_SHOT);
    ads[i]->setVoltageReference(REF_EXTERNAL);
    ads[i]->setGain(GAIN_ONE);
    ads[i]->setDataRate(90);
    pinMode(adcDataReadyPins[i], INPUT_PULLUP);
    ads[i]->calibrate();
  }

  Serial.println("Initializing Libraries");
  Serial.flush();

  Solenoids::init();
  batteryMonitor::init();

  Ducers::init(numPressureTransducers, ptAdcIndices, ptAdcChannels, ads);
  Thermocouple::Analog::init(numAnalogThermocouples, thermAdcIndices, thermAdcChannels, ads);
  Thermocouple::Cryo::init(numCryoTherms, cryoThermAddrs, cryoTypes);

  tempController::init(10, 2, 7); // setPoint = 10 C, alg = PID, heaterPin = 7
}

void loop() {
  if (RFSerial.available() > 0) {
    int i = 0;
    while (RFSerial.available()) {
      command[i] = RFSerial.read();
      Serial.print(command[i]);
      i++;
    }
    Serial.println();
    Serial.println(String(command));
    int action = decode_received_packet(String(command), &valve, &valves, numValves);
    if (action != -1) {
      take_action(&valve, action);
      packet = make_packet(valve.id, false);
      Serial.println(packet);
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
    sensor = sensors[j];
    board_address = sensor.board_address;
    sensor_id = sensor.id;
    sensorReadFunc(sensor.id);
    packet = make_packet(sensor.id, false);
    Serial.println(packet);
    RFSerial.println(packet);
    write_to_SD(packet.c_str(), file_name);
  }
}


/**
 *
 */
void sensorReadFunc(int id) {
  switch (id) {
    case 0:
      Thermocouple::Analog::readTemperatureData(farrbconvert.sensorReadings);
      farrbconvert.sensorReadings[1] = tempController::controlTemp(farrbconvert.sensorReadings[0]);
      farrbconvert.sensorReadings[2] = -1;
      break;
    case 1:
      Ducers::readAllPressures(farrbconvert.sensorReadings);
      break;
    case 2:
      batteryMonitor::readAllBatteryStats(farrbconvert.sensorReadings);
      break;
    case 4:
      Thermocouple::Cryo::readCryoTemps(farrbconvert.sensorReadings);
      //farrbconvert.sensorReadings[1]=0;
      farrbconvert.sensorReadings[2]=0;
      farrbconvert.sensorReadings[3]=0;
      farrbconvert.sensorReadings[4]=-1;
      break;
    default:
      Serial.println("some other sensor");
      break;
  }
}
