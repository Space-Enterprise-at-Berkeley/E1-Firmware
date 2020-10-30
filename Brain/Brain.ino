/*
 * Brain_I2C.ino - A c++ program that uses I2C to establish communication between
 * the sensors and valves inside to the rocket with the ground station. Able to send
 * data to the ground station via RF. Can receive and process commands sent from
 * ground station.
 * Created by Vainavi Viswanath, Aug 21, 2020.
 */

#include <Wire.h>
#include "brain_utils.h"
#include <GPS.h>
#include <Barometer.h>
#include <ducer.h>
#include <SD.h>
#include <SPI.h>
#include <TimeLib.h>

#define RFSerial Serial2
#define GPSSerial Serial3


// within loop state variables
int board_address = 0;
byte sensor_id = 0;
uint8_t val_index = 0;
String command = "";
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

/*
 * Array of all sensors we would like to get data from.
 */
sensorInfo all_ids[8] = {
  {"Low Pressure",8,0,1,1,NULL} //example
};

sensorInfo sensor = {"",0,0,0,0, NULL};

int numSensors = 1;

/*
 *  Stores how often we should be requesting data from each sensor.
 */
int sensor_checks[sizeof(all_ids)/sizeof(sensorInfo)][2];

valveInfo valve  = {"",0, 0, 0};

void setup() {
  Wire.begin();
  Serial.begin(9600);
  RFSerial.begin(57600);
  for (int i=0; i<sizeof(all_ids)/sizeof(sensorInfo); i++) {
    sensor_checks[i][0] = all_ids[i].clock_freq;
    sensor_checks[i][1] = 1;
  }

  Ducers::init(&Wire);
  Barometer::init(&Wire);
  GPS::init(&GPSSerial);
}

void loop() {
  if(RFSerial.available()) {
    command = RFSerial.readString();
    int action = decode_received_packet(command, &valve);
    take_action(&valve, action);
  }

  /*
   * Code for requesting data and relaying back to ground station
   */
  for (int j = 0; j < sizeof(all_ids)/sizeof(sensorInfo); j++) {
    if (sensor_checks[j][0] == sensor_checks[j][1]) {
      sensor_checks[j][1] = 1;
    } else {
      sensor_checks[j][1] += 1;
      continue;
    }
    sensor = all_ids[j];
    board_address = sensor.board_address;
    sensor_id = sensor.sensor_id;
    if(sensor_id != -1) {
      Wire.beginTransmission(board_address);
      Wire.write(sensor_id);
      Wire.endTransmission();
      Wire.requestFrom(board_address, 24);

      val_index = 0;
      while (Wire.available()){
        farrbconvert.buffer[val_index] = Wire.read();
        val_index++;
      }
    } else {
      sensor.dataReadFunc(farrbconvert.sensorReadings);
    }
    String packet = make_packet(sensor);
    //Serial.println(packet);
    RFSerial.println(packet);
    bool did_write = write_to_SD(packet);
  }
  delay(100);
}


bool write_to_SD(String message){
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
