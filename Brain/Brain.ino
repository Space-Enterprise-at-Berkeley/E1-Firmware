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

#define RFSerial Serial2
#define GPSSerial Serial3


// within loop state variables
int board_address = 0;
byte sensor_id = 0;
uint8_t val_index = 0;

/*
 * Array of all sensors we would like to get data from.
 */
sensorInfo all_ids[11] = {
  // local sensors
   {"LOX Injector Low Pressure",  -1, -1, 1, 1, &(Ducers::readLOXInjectorPressure)},
   {"Prop Injector Low Pressure", -1, -1, 2, 1, &(Ducers::readPropaneInjectorPressure)},
   {"LOX Tank Low Pressure",      -1, -1, 3, 1, &(Ducers::readLOXTankPressure)},
   {"Prop Tank Low Pressure",     -1, -1, 4, 1, &(Ducers::readPropaneTankPressure)},
   {"High Pressure",              -1, -1, 5, 2, &{Ducers::readHighPressure)},
   {"Temperature",                -1, -1, 6, 3, NULL},
   {"GPS",                        -1, -1, 7, 5, &(GPS::readPositionData)},
   {"GPS Aux",                    -1, -1, 8, 8, &(GPS::readAuxilliaryData)},
   {"Barometer",                  -1, -1, 8, 6, &(Barometer::readAltitudeData)},
   {"Load Cell Engine Left",      -1, -1, 9,  5, NULL},
   {"Load Cell Engine Right",     -1, -1, 10, 5, NULL}
};

sensorInfo sensor {"", 0, 0, 0, 0, 0};

/*
 *  Stores how often we should be requesting data from each sensor.
 */
int sensor_checks[sizeof(all_ids)/sizeof(sensorInfo)][2];

valveInfo valve_ids[7] = {
  valveInfo("LOX 2 Way", 20),
  valveInfo("LOX 5 Way", 21),
  valveInfo("LOX GEMS", 22),
  valveInfo("Propane 2 Way", 23),
  valveInfo("Propane 5 Way", 24),
  valveInfo("Propane GEMS", 25),
  valveInfo("High Pressure Solenoid", 26)
};

valveInfo valve = valveInfo("",0);

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
  String command = "";
//  command = RFSerial.readString();
  command = "{26,1|456}";
  int action = decode_received_packet(command, &valve);
  take_action(&valve, action);

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
  }
  delay(100);
}


bool write_to_SD(String message){
  // every reading that we get from sensors should be written to sd and saved.
  // TODO: Someone's code here
  return false;
}
