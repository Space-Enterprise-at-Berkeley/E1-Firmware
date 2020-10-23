/*
 * Brain_I2C.ino - A c++ program that uses I2C to establish communication between 
 * the sensors and valves inside to the rocket with the ground station. Able to send
 * data to the ground station via RF. Can receive and process commands sent from 
 * ground station. 
 * Created by Vainavi Viswanath, Aug 21, 2020.
 */

#include <i2c_driver_wire.h> // https://github.com/Richard-Gemmell/teensy4_i2c
#include "brain_utils.h"
//#include <GPS.h>
//#include <Barometer.h>
//#include <ducer.h>
#include <Thermocouple.h>

#define RFSerial Serial6
//#define GPSSerial Serial8

// within loop state variables
int board_address = 0;
int sensor_id = 0;
uint8_t val_index = 0;
String command = "";

  

void testTempRead(float *data) {
  Thermocouple::readTemperatureData(data);
}

/*
 * Array of all sensors we would like to get data from.
 */
const int numSensors = 11; // can use sizeof(all_ids)/sizeof(sensorInfo)

sensorInfo all_ids[numSensors] = {
  // local sensors
   {"LOX Injector Low Pressure",  8, -1, 1, 1, NULL}, //&(Ducers::readLOXInjectorPressure)},
   {"Prop Injector Low Pressure", 8, 2, 2, 1, NULL}, //&(Ducers::readPropaneInjectorPressure)},
   {"LOX Tank Low Pressure",      8, 3, 3, 1, NULL}, //&(Ducers::readLOXTankPressure)},
   {"Prop Tank Low Pressure",     8, 4, 4, 1, NULL}, //&(Ducers::readPropaneTankPressure)},
   {"High Pressure",              8, 5, 5, 2, NULL}, //&(Ducers::readHighPressure)},
   {"Temperature",                -1, -1, 6, 3, NULL}, //&(testTempRead)}, //&(Thermocouple::readTemperatureData)},
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
int sensor_checks[numSensors][2];

valveInfo valve  = {"",0, 0, 0};

void setup() {
  Wire.setClock(100 * 1000);   // Set the clock speed before calling begin()
  Wire.begin();
  Serial.begin(9600);
  RFSerial.begin(57600);
  
  delay(1000);
  
  Serial.println("setup 1");

  
  for (int i=0; i<numSensors; i++) {
    sensor_checks[i][0] = all_ids[i].clock_freq;
    sensor_checks[i][1] = 1;
  }
  Serial.println("setup");

  Thermocouple::init(1, 11);
////  Ducers::init(&Wire);
////  Barometer::init(&Wire);
////  GPS::init(&GPSSerial);
}

void loop() {
  Serial.println("top of loop");

  if(RFSerial.available()) {
    //command = RFSerial.readString();
    command = "{21,1|E5C0}";
    int action = decode_received_packet(command, &valve);
    take_action(&valve, action);
    Serial.print("got command");
  }

  /*
   * Code for requesting data and relaying back to ground station
   */
  for (int j = 0; j < numSensors; j++) {
    Serial.println("Inner for loop");
//    if (sensor_checks[j][0] == sensor_checks[j][1]) {
//      sensor_checks[j][1] = 1;
//    } else {
//      sensor_checks[j][1] += 1;
//      continue;
//    }
    sensor = all_ids[j];
    board_address = sensor.board_address;
    sensor_id = sensor.sensor_id;
    Serial.print("sensor id: ");
    Serial.print(sensor_id);
    Serial.println(", " + sensor.name);
    
    if (sensor_id != -1) {
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
      for (int i = val_index; i < 24; i++){
        farrbconvert.buffer[i] = 0;
      }
    } else {
//      sensor.dataReadFunc(farrbconvert.sensorReadings);
      sensorReadFunc(sensor.overall_id);
    }
//    
    String packet = make_packet(sensor);
    Serial.println(packet);
//    RFSerial.println(packet);
  }
  delay(100);
}

void sensorReadFunc(int id) {
  switch(id){
    case 1:
      Ducers::readLOXInjectorPressure(farrbconvert.sensorReadings);
      break;
    case 6:
      Thermocouple::setSensor(0);
      Thermocouple::readTemperatureData(farrbconvert.sensorReadings);
      break;
    default:
      //Serial.println("some other, not temp sensor");  
      break;
  }
}


bool write_to_SD(String message){
  // every reading that we get from sensors should be written to sd and saved.
  // TODO: Someone's code here
  return false;
}
