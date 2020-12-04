/*
   Brain_I2C.ino - A c++ program that uses I2C to establish communication between
   the sensors and valves inside to the rocket with the ground station. Able to send
   data to the ground station via RF. Can receive and process commands sent from
   ground station.
   Created by Vainavi Viswanath, Aug 21, 2020.
*/

#include <Wire.h>
#include "brain_utils.h"
//#include <GPS.h>
//#include <Barometer.h>

#define RFSerial Serial6
#define GPSSerial Serial8
#include <SD.h>
#include <SPI.h>
#include <TimeLib.h>

#define FLIGHT_BRAIN_ADDR 0x00

// within loop state variables
uint8_t board_address = 0;
uint8_t sensor_id = 0;
uint8_t val_index = 0;
char command[50]; //input command from GS

const uint8_t numSensors = 8;

/*
   Array of all sensors we would like to get data from.
*/
sensorInfo sensors[numSensors] = {
  // local sensors
  {"Temperature",                FLIGHT_BRAIN_ADDR, 0, 3}, //&(testTempRead)}, //&(Thermocouple::readTemperatureData)},
  {"All Pressure",               FLIGHT_BRAIN_ADDR, 1, 1},
  {"Battery Stats",              FLIGHT_BRAIN_ADDR, 2, 3},
//  {"Load Cells",                 FLIGHT_BRAIN_ADDR, 3, 5},
  {"Aux temp",                   FLIGHT_BRAIN_ADDR, 4, 5},


//  {"Solenoid Ack",               FLIGHT_BRAIN_ADDR, 4, -1},
//  {"Recovery Ack",               FLIGHT_BRAIN_ADDR, 5, -1},

  //  {"GPS",                        -1, -1, 7, 5, NULL}, //&(GPS::readPositionData)},
  //  {"GPS Aux",                    -1, -1, 8, 8, NULL}, //&(GPS::readAuxilliaryData)},
  //  {"Barometer",                  -1, -1, 8, 6, NULL}, //&(Barometer::readAltitudeData)},
  //  {"Load Cell Engine Left",      -1, -1, 9,  5, NULL},
  //  {"Load Cell Engine Right",     -1, -1, 10, 5, NULL}
};

/*
    Stores how often we should be requesting data from each sensor.
*/
int sensor_checks[numSensors][2];

valveInfo valve;
sensorInfo sensor = {"temp", 23, 23, 23};

long startTime;
String packet;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  RFSerial.begin(57600);
  GPSSerial.begin(4608000);

  delay(1000);

  for (int i = 0; i < numSensors; i++) {
    sensor_checks[i][0] = sensors[i].clock_freq;
    sensor_checks[i][1] = 1;
  }

  Recovery::init();
  Solenoids::init();
  Ducers::init(&Wire);
  batteryMonitor::init();

  Thermocouple::Cryo::init(numCryoTherms, cryoThermAddrs);
  tempController::init(10, 2, 7); // setPoint = 10 C, alg = PID, heaterPin = 7
  ////  Barometer::init(&Wire);
  ////  GPS::init(&GPSSerial);
}

void loop() {
  if (Serial.available() > 0) {
    int i = 0;
    while (Serial.available()) {
      command[i] = Serial.read();
      Serial.print(command[i]);
      i++;
    }
    Serial.println();
    Serial.println(String(command));
    int action = decode_received_packet(String(command), &valve);
    if (action != -1) {
      take_action(&valve, action);
      packet = make_packet(valve.id);
      Serial.println(packet);
      RFSerial.println(packet);
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
    packet = make_packet(sensor.id);
    Serial.println(packet);
    RFSerial.println(packet);
    //bool did_write = write_to_SD(packet);
  }
}


//String file_name = "E1_" + String(month()) + "/" + String(day()) + "/" + String(year()) + "_" + String(hour()) + ":" + String(minute()) + ":" + String(second()) + "_log.txt";
//
//char packet0[] PROGMEM = "Packet0";
//char packet1[] PROGMEM = "Packet1";
//char packet2[] PROGMEM = "Packet2";
//char packet3[] PROGMEM = "Packet3";
//char packet4[] PROGMEM = "Packet4";
//char packet5[] PROGMEM = "Packet5";
//char packet6[] PROGMEM = "Packet6";
//char packet7[] PROGMEM = "Packet7";
//char packet8[] PROGMEM = "Packet8";
//char packet9[] PROGMEM = "Packet9";
//
//int bfr_idx = 0;
//char buffer[64];
//char* packet_table[] PROGMEM = {packet0, packet1, packet2, packet3, packet4, packet5, packet6, packet7, packet8, packet9};
//
//bool write_to_SD(String message) {
//  if (!SD.begin(BUILTIN_SDCARD))
//    return false;
//
//  packet_table[bfr_idx] = message.c_str();
//  bfr_idx++;
//
//  if (bfr_idx == 10) {
//    File myFile = SD.open(file_name.c_str(), FILE_WRITE);
//
//    if (myFile) {                                                     //If the file opened
//      for (int i = 0; i <= bfr_idx; i++) {
//        strcpy_P(buffer, (char *)pgm_read_word(&(packet_table[i])));  // Necessary casts and dereferencing, just copy.
//        myFile.println(buffer);
//      }
//      myFile.close();
//      bfr_idx = 0;
//      return true;
//    }
//    else {                                                            //If the file didn't open
//      return false;
//    }
//  }
//}
