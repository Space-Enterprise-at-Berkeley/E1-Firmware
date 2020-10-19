/*
 * Brain_I2C.ino - A c++ program that uses I2C to establish communication between 
 * the sensors and valves inside to the rocket with the ground station. Able to send
 * data to the ground station via RF. Can receive and process commands sent from 
 * ground station. 
 * Created by Vainavi Viswanath, Aug 21, 2020.
 */

#include <Wire.h>
#include "brain_utils.h"
//#include <GPS.h>
//#include <Barometer.h>
//#include <ducer.h>

#define RFSerial Serial6
//#define GPSSerial Serial8

// within loop state variables
int board_address = 0;
int sensor_id = 0;
uint8_t val_index = 0;
String command = "";
  
/*
 * Array of all sensors we would like to get data from.
 */
sensorInfo all_ids[11] = {
  // local sensors
   {"LOX Injector Low Pressure",  -1, -1, 1, 1, NULL}, //&(Ducers::readLOXInjectorPressure)},
   {"Prop Injector Low Pressure", -1, -1, 2, 1, NULL}, //&(Ducers::readPropaneInjectorPressure)},
   {"LOX Tank Low Pressure",      -1, -1, 3, 1, NULL}, //&(Ducers::readLOXTankPressure)},
   {"Prop Tank Low Pressure",     -1, -1, 4, 1, NULL}, //&(Ducers::readPropaneTankPressure)},
   {"High Pressure",              -1, -1, 5, 2, NULL}, //&(Ducers::readHighPressure)},
   {"Temperature",                -1, -1, 6, 3, NULL},
   {"GPS",                        -1, -1, 7, 5, NULL}, //&(GPS::readPositionData)},
   {"GPS Aux",                    -1, -1, 8, 8, NULL}, //&(GPS::readAuxilliaryData)},
   {"Barometer",                  -1, -1, 8, 6, NULL}, //&(Barometer::readAltitudeData)},
   {"Load Cell Engine Left",      -1, -1, 9,  5, NULL},
   {"Load Cell Engine Right",     -1, -1, 10, 5, NULL}
};

sensorInfo sensor = {"temp", 23, 23, 23, 23, NULL};

/*
 *  Stores how often we should be requesting data from each sensor.
 */
int sensor_checks[sizeof(all_ids)/sizeof(sensorInfo)][2];

valveInfo valve  = {"",0, 0, 0};

void setup() {
  Wire.begin();       
  Serial.begin(9600);
  RFSerial.begin(57600);
  
  delay(1000);
  
  RFSerial.println("setup 1");
  Serial.println("setup 1");

  
  for (int i=0; i<sizeof(all_ids)/sizeof(sensorInfo); i++) {
    sensor_checks[i][0] = all_ids[i].clock_freq;
    sensor_checks[i][1] = 1;
  }
  RFSerial.println("setup");
  Serial.println("setup");

  
  farrbconvert.sensorReadings[0] = 98;
  farrbconvert.sensorReadings[1] = 45;
  farrbconvert.sensorReadings[2] = 16;
  farrbconvert.sensorReadings[3] = 0;
  farrbconvert.sensorReadings[4] = -1;
  farrbconvert.sensorReadings[5] = -1;
  RFSerial.println(make_packet(sensor));
  Serial.println(make_packet(sensor));


//  Ducers::init(&Wire);
//  Barometer::init(&Wire);
//  GPS::init(&GPSSerial);
}

void loop() {
  RFSerial.println("top of loop");
  Serial.println("top of loop");

  if(RFSerial.available()) {
    //command = RFSerial.readString();
    command = "{21,1|E5C0}";
    int action = decode_received_packet(command, &valve);
    take_action(&valve, action);
    RFSerial.print("got command");
    Serial.print("got command");
  }

  /*
   * Code for requesting data and relaying back to ground station
   */
  for (int j = 0; j < sizeof(all_ids)/sizeof(sensorInfo); j++) {
    Serial.println("Inner for loop");
    if (sensor_checks[j][0] == sensor_checks[j][1]) {
      sensor_checks[j][1] = 1;
    } else {
      sensor_checks[j][1] += 1;
      continue;
    }
    sensor = all_ids[j];
    board_address = sensor.board_address;
    sensor_id = sensor.sensor_id;
    Serial.print("sensor id: ");
    Serial.println(sensor_id);

    if (sensor_id != -1) {
      Serial.println("doing i2c request");

//      Wire.beginTransmission(board_address);
//      Wire.write(sensor_id);
//      Wire.endTransmission();
//      Wire.requestFrom(board_address, 24); 
      val_index = 0;
//      while (Wire.available()) {
//        farrbconvert.buffer[val_index] = Wire.read();
//        val_index++;
//      }
    } else {
      sensor.dataReadFunc(farrbconvert.sensorReadings);
      Serial.println("read straight from board");

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
  return false;
}
