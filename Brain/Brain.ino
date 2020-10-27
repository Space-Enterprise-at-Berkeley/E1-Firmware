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

#define FLIGHT_BRAIN_ADDR 0x00

// within loop state variables
int board_address = 0;
int sensor_id = 0;
uint8_t val_index = 0;
String command = "";


/*
   Array of all sensors we would like to get data from.
*/
const int numSensors = 7; // can use sizeof(all_ids)/sizeof(sensorInfo)

sensorInfo sensors[numSensors] = {
  // local sensors
  {"LOX Injector Low Pressure",  FLIGHT_BRAIN_ADDR, 1, 1}, //&(Ducers::readLOXInjectorPressure)},
  {"Prop Injector Low Pressure", FLIGHT_BRAIN_ADDR, 2, 1}, //&(Ducers::readPropaneInjectorPressure)},
  {"LOX Tank Low Pressure",      FLIGHT_BRAIN_ADDR, 3, 1}, //&(Ducers::readLOXTankPressure)},
  {"Prop Tank Low Pressure",     FLIGHT_BRAIN_ADDR, 4, 1}, //&(Ducers::readPropaneTankPressure)},
  {"Pressurant Tank",            FLIGHT_BRAIN_ADDR, 5, 2}, //&(Ducers::readHighPressure)},
  {"Temperature",                FLIGHT_BRAIN_ADDR, 6, 3}, //&(testTempRead)}, //&(Thermocouple::readTemperatureData)},
//  {"All Pressure",               FLIGHT_BRAIN_ADDR, 7, 1},
  {"Battery Stats",              FLIGHT_BRAIN_ADDR, 8, 5},
//  {"GPS",                        -1, -1, 7, 5, NULL}, //&(GPS::readPositionData)},
//  {"GPS Aux",                    -1, -1, 8, 8, NULL}, //&(GPS::readAuxilliaryData)},
//  {"Barometer",                  -1, -1, 8, 6, NULL}, //&(Barometer::readAltitudeData)},
//  {"Load Cell Engine Left",      -1, -1, 9,  5, NULL},
//  {"Load Cell Engine Right",     -1, -1, 10, 5, NULL}
};

sensorInfo sensor = {"temp", 23, 23, 23};


/*
    Stores how often we should be requesting data from each sensor.
*/
int sensor_checks[numSensors][2];

valveInfo valve  = {"", 0, 0, 0};

void setup() {
  Wire.begin();
  Serial.begin(9600);
  RFSerial.begin(57600);

  delay(1000);

  Serial.println("setup 1");

  for (int i = 0; i < numSensors; i++) {
    sensor_checks[i][0] = sensors[i].clock_freq;
    sensor_checks[i][1] = 1;
  }
  Serial.println("setup");

  Thermocouple::init(1, 11);
  tempController::init(20, 2, 7); // setPoint = 20 C, alg = PID, heaterPin = 7
  Ducers::init(&Wire);
  batteryMonitor::init();
  ////  Barometer::init(&Wire);
  ////  GPS::init(&GPSSerial);
}

void loop() {
  Serial.println("top of loop");

  if (RFSerial.available()) {
    Serial.println("got command");
    command = RFSerial.readStringUntil('}');
    Serial.println(command);
    int action = decode_received_packet(command, &valve);
    take_action(valve.id, action);
    Serial.print("got command");
  }

  /*
     Code for requesting data and relaying back to ground station
  */
  for (int j = 0; j < numSensors; j++) {
    //    if (sensor_checks[j][0] == sensor_checks[j][1]) {
    //      sensor_checks[j][1] = 1;
    //    } else {
    //      sensor_checks[j][1] += 1;
    //      continue;
    //    }
    sensor = sensors[j];
    board_address = sensor.board_address;
    sensor_id = sensor.id;
//    Serial.print("sensor id: ");
//    Serial.print(sensor_id);
//    Serial.println(", " + sensor.name);

    if (board_address != FLIGHT_BRAIN_ADDR) {
      // Don't worry about this code. Vainavi is handling this.
      Serial.println("doing i2c request");

      Wire.beginTransmission(board_address);
      Serial.println(Wire.write(sensor_id));
      Serial.println("wrote 1 byte");
      delay(100);
      Serial.println("delayed");
      Serial.println(Wire.endTransmission());

      Serial.println(Wire.requestFrom(board_address, 24));
      val_index = 0;
      while (Wire.available()) {
        Serial.println("wire available, reading");
        farrbconvert.buffer[val_index] = Wire.read();
        val_index++;
      }
      for (int i = val_index; i < 24; i++) {
        farrbconvert.buffer[i] = 0;
      }
    } else {
      sensorReadFunc(sensor.id);
    }
    String packet = make_packet(sensor);
    Serial.println(packet);
    RFSerial.println(packet);
  }
  delay(10);
}

void sensorReadFunc(int id) {
  switch (id) {
    case 1:
      Ducers::readLOXInjectorPressure(farrbconvert.sensorReadings);
      break;
    case 2:
      Ducers::readPropaneInjectorPressure(farrbconvert.sensorReadings);
      break;
    case 3:
      Ducers::readLOXTankPressure(farrbconvert.sensorReadings);
      break;
    case 4:
      Ducers::readPropaneTankPressure(farrbconvert.sensorReadings);
      break;
    case 5:
      Ducers::readPressurantTankPressure(farrbconvert.sensorReadings);
      break;
    case 7:

      break;
    case 8:
      batteryMonitor::readAllBatteryStats(farrbconvert.sensorReadings);
      break;
    case 6:
      Thermocouple::setSensor(0);
      Thermocouple::readTemperatureData(farrbconvert.sensorReadings);
//      Serial.println(farrbconvert.sensorReadings[0]);
      farrbconvert.sensorReadings[1] = tempController::controlTemp(farrbconvert.sensorReadings[0]);
//      Serial.println(farrbconvert.sensorReadings[1]);
      farrbconvert.sensorReadings[2] = -1;
      break;
    default:
      //Serial.println("some other, not temp sensor");
      break;
  }
}


bool write_to_SD(String message) {
  // every reading that we get from sensors should be written to sd and saved.
  // TODO: Someone's code here
  return false;
}
