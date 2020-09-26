/*
 * Brain_I2C.ino - A c++ program that uses I2C to establish communication between
 * the sensors and valves inside to the rocket with the ground station. Able to send
 * data to the ground station via RF. Can receive and process commands sent from
 * ground station.
 * Created by Vainavi Viswanath, Aug 21, 2020.
 */

#include <Wire.h>
//#include <SoftwareSerial.h>
#include "brain_utils.h"
#include "GPS.h"
#include "Barometer.h"

#define TIMER_INTERRUPT_DEBUG      0

#define USE_TIMER_1     true
#define USE_TIMER_2     true
#define USE_TIMER_3     false
#define USE_TIMER_4     false
#define USE_TIMER_5     false

#include "TimerInterrupt.h"


Serial2 RFSerial(2,3);

int board_address = 0;
byte sensor_id = 0;
uint8_t index = 0;

// Initialize sensor libs
GPS gps(&Wire);
Barometer baro(&Wire);


/*
 * Array of all sensors we would like to get data from.
 */
sensorInfo all_ids[10] = {
  // local sensors
  sensorInfo("LoX Injector Low Pressure",8,0,1,1, NULL),
  sensorInfo("Prop Injector Low Pressure",8,1,2,1, NULL),
  sensorInfo("LoX Tank Low Pressure",8,3,3,1, NULL),
  sensorInfo("Prop Tank Low Pressure",8,4,4,1, NULL),
  sensorInfo("High Pressure",8,5,5,2, NULL),
  sensorInfo("Temperature",8,-1,6,3, ),
  sensorInfo("GPS",8,-1,7,5, ),
  sensorInfo("Barometer",8,-1,8,6, ),

  // ground side board
  sensorInfo("Load Cell Engine Left", 5, 0, 9, 5, NULL),
  sensorInfo("Load Cell Engine Right", 5, 0, 10, 5, NULL),
};

sensorInfo sensor = sensorInfo("",0,0,0,0, NULL);

/*
 *  Stores how often we should be requesting data from each sensor.
 */
int sensor_checks[sizeof(all_ids)/sizeof(sensorInfo)][2];

void setup() {
  Wire.begin();
  Serial.begin(9600);
  RFSerial.begin(57600);
  for (int i=0; i<sizeof(all_ids)/sizeof(sensorInfo); i++) {
    sensor_checks[i][0] = all_ids[i].clock_freq;
    sensor_checks[i][1] = 1;
  }
}

void loop() {
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

      index = 0;
      while (Wire.available()){
        farrbconvert.buffer[index] = Wire.read();
        index++;
      }
    } else {
      sensor.readDataFunc(farrbconvert.sensorReadings);
    }
    for (int i=0; i<6; i++) {
      float reading = farrbconvert.sensorReadings[i];
      if (reading > 0) {
      }
    }
    String packet = make_packet(sensor);
    Serial.println(packet);
    RFSerial.println(packet);
  }
  delay(100);
}

bool write_to_SD(String message){
  // every reading that we get from sensors should be written to sd and saved.
  // TODO: Someone's code here
}
