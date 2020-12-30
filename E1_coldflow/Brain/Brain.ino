/*
   Brain_I2C.ino - A c++ program that uses I2C to establish communication between
   the sensors and valves inside to the rocket with the ground station. Able to send
   data to the ground station via RF. Can receive and process commands sent from
   ground station.
   Created by Vainavi Viswanath, Aug 21, 2020.
*/

#include <Wire.h>
#include "brain_utils.h"
#include <GPS.h>
#include <Barometer.h>
#include <ducer.h>
//#include <SD.h>
#include <SPI.h>
#include <TimeLib.h>

#define RFSerial Serial6
#define GPSSerial Serial8

// within loop state variables
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

void setup() {
  Wire.begin();
  Serial.begin(57600);
  RFSerial.begin(57600);
  GPSSerial.begin(4608000);

  delay(1000);

  for (int i = 0; i < numSensors; i++) {
    sensor_checks[i][0] = sensors[i].clock_freq;
    sensor_checks[i][1] = 1;
  }

  int res = sd.begin(SdioConfig(FIFO_SDIO));
  if (!res) {
      // make packet with this status. print, and send over rf
      // do additional error handling; what if no sd?
  }
  file.open(file_name, O_RDWR | O_CREAT);
  file.close();
  sdBuffer = new Queue();

  Recovery::init();
  Solenoids::init();
  Ducers::init(&Wire);
  batteryMonitor::init();

  Thermocouple::Cryo::init(numCryoTherms, cryoThermAddrs, cryoTypes);
  tempController::init(10, 2, 7); // setPoint = 10 C, alg = PID, heaterPin = 7
  ////  Barometer::init(&Wire);
  ////  GPS::init(&GPSSerial);

  //test SD card is there
  String start = "beginning writing data";
  if(!write_to_SD(start.c_str())){
    packet = make_packet(101, true);
    RFSerial.println(packet);
  }
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
    int action = decode_received_packet(String(command), &valve, &sensor);
    if (action != -1) {
      take_action(&valve, &sensor, action);
      if (action == 2) {
        packet = make_packet(sensor.id, false);
      } else {
        packet = make_packet(valve.id, false);
      }
      Serial.println(packet);
      RFSerial.println(packet);
      write_to_SD(packet.c_str());
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
    take_action(&valve, &sensor, 2);
    packet = make_packet(sensor.id, false);
    Serial.println(packet);
    RFSerial.println(packet);
    write_to_SD(packet.c_str());
  }
}
