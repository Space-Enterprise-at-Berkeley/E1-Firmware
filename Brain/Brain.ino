/*
   Brain_I2C.ino - A c++ program that uses I2C to establish communication between
   the sensors and valves inside to the rocket with the ground station. Able to send
   data to the ground station via RF. Can receive and process commands sent from
   ground station.
   Created by Vainavi Viswanath, Aug 21, 2020.
*/

#include <Wire.h> // https://github.com/Richard-Gemmell/teensy4_i2c
#include "brain_utils.h"
//#include <GPS.h>
//#include <Barometer.h>
#include <ducer.h>
#include <Thermocouple.h>
#include <tempController.h>
#include <batteryMonitor.h>

#define RFSerial Serial6
//#define GPSSerial Serial8
#include <SD.h>
#include <SPI.h>
#include <TimeLib.h>

#define FLIGHT_BRAIN_ADDR 0x00

// within loop state variables
int board_address = 0;
int sensor_id = 0;
uint8_t val_index = 0;
char command[50]; //input command from GS

/*
   Array of all sensors we would like to get data from.
*/
const int numSensors = 8; // can use sizeof(all_ids)/sizeof(sensorInfo)

sensorInfo sensors[numSensors] = {
  // local sensors
  {"Temperature",                FLIGHT_BRAIN_ADDR, 0, 3}, //&(testTempRead)}, //&(Thermocouple::readTemperatureData)},
  {"All Pressure",               FLIGHT_BRAIN_ADDR, 1, 1},
  {"Battery Stats",              FLIGHT_BRAIN_ADDR, 2, 3},
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

void setup() {
  Wire.begin();
  Serial.begin(9600);
  RFSerial.begin(57600);

  delay(1000);

  for (int i = 0; i < numSensors; i++) {
    sensor_checks[i][0] = sensors[i].clock_freq;
    sensor_checks[i][1] = 1;
  }

  Solenoids::init();
  Ducers::init(&Wire);
  batteryMonitor::init();
//  Thermocouple::init();
  tempController::init(10, 2, 7); // setPoint = 20 C, alg = PID, heaterPin = 7
  ////  Barometer::init(&Wire);
  ////  GPS::init(&GPSSerial);
}

void loop() {
  if (RFSerial.available() > 0) {
    int i = 0;
    while(RFSerial.available()) {
      command[i] = RFSerial.read();
      i++;
    }
    int action = decode_received_packet(String(command), &valve);
    take_action(&valve, action);
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

    if (board_address != FLIGHT_BRAIN_ADDR) {
//      // Don't worry about this code. Vainavi is handling this.
//      Wire.beginTransmission(board_address);
//      //delay(100);
//      val_index = 0;
//      while (Wire.available()) {
//        farrbconvert.buffer[val_index] = Wire.read();
//        val_index++;
//      }
//      for (int i = val_index; i < 24; i++) {
//        farrbconvert.buffer[i] = 0;
//      }.
    } else {
      sensorReadFunc(sensor.id);
    }

    String packet = make_packet(sensor);
    Serial.println(packet);
    RFSerial.println(packet);
    //bool did_write = write_to_SD(packet);
  }
}

void sensorReadFunc(int id) {
  switch (id) {
    case 0:
      //Thermocouple::setSensor(0);
      Ducers::readTemperatureData(farrbconvert.sensorReadings);
      farrbconvert.sensorReadings[1] = tempController::controlTemp(farrbconvert.sensorReadings[0]);
      farrbconvert.sensorReadings[2] = -1;
      break;
    case 1:
      Ducers::readAllPressures(farrbconvert.sensorReadings);
      break;
    case 2:
      batteryMonitor::readAllBatteryStats(farrbconvert.sensorReadings);
      break;
    default:
      Serial.println("some other sensor");
      break;
  }
}


String file_name = "E1_" + String(month()) + "/" + String(day()) + "/" + String(year()) + "_" + String(hour()) + ":" + String(minute()) + ":" + String(second()) + "_log.txt";

char packet0[] PROGMEM = "Packet0";
char packet1[] PROGMEM = "Packet1";
char packet2[] PROGMEM = "Packet2";
char packet3[] PROGMEM = "Packet3";
char packet4[] PROGMEM = "Packet4";
char packet5[] PROGMEM = "Packet5";
char packet6[] PROGMEM = "Packet6";
char packet7[] PROGMEM = "Packet7";
char packet8[] PROGMEM = "Packet8";
char packet9[] PROGMEM = "Packet9";


int bfr_idx = 0;
char buffer[64];
char* packet_table[] PROGMEM = {packet0, packet1, packet2, packet3, packet4, packet5, packet6, packet7, packet8, packet9};

bool write_to_SD(String message) {
  // every reading that we get from sensors should be written to sd and saved.
  // TODO: Someone's code here
    if (!SD.begin(BUILTIN_SDCARD))
        return false;

    packet_table[bfr_idx] = message.c_str();
    bfr_idx++;
    
    if(bfr_idx == 10) {
      File myFile = SD.open(file_name.c_str(), FILE_WRITE);

      if(myFile) {                                                      //If the file opened
        for(int i = 0; i <= bfr_idx; i++){
          strcpy_P(buffer, (char *)pgm_read_word(&(packet_table[i])));  // Necessary casts and dereferencing, just copy.
          myFile.println(buffer);
        }
        myFile.close();
        bfr_idx = 0;
        return true;
      } 
      else {                                                            //If the file didn't open
        return false;
      }
    }
}
