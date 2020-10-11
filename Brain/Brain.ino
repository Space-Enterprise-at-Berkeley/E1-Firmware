/*
 * Brain_I2C.ino - A c++ program that uses I2C to establish communication between
 * the sensors and valves inside to the rocket with the ground station. Able to send
 * data to the ground station via RF. Can receive and process commands sent from
 * ground station.
 * Created by Vainavi Viswanath, Aug 21, 2020.
 */

#include <Wire.h>
#include "brain_utils.h"

#include "GPS.h"
#include "Barometer.h"

#define RFSerial Serial2
#define GPSSerial Serial3

// Initialize sensor libs
//GPS gps(&GPSSerial);
//Barometer baro(&Wire);


// within loop state variables
int board_address = 0;
int sensor_id = 0;
int currIndex = 0;

/*
 * Array of all sensors we would like to get data from.
 */
sensorInfo all_ids[11] = {
  // local sensors
  sensorInfo("LoX Injector Low Pressure",  -1, -1, 1, 1, NULL),
  sensorInfo("Prop Injector Low Pressure", -1, -1, 2, 1, NULL),
  sensorInfo("LoX Tank Low Pressure",      -1, -1, 3, 1, NULL),
  sensorInfo("Prop Tank Low Pressure",     -1, -1, 4, 1, NULL),
  sensorInfo("High Pressure",              -1, -1, 5, 2, NULL),
  sensorInfo("Temperature",                -1, -1, 6, 3, NULL),
  sensorInfo("GPS",                        -1, -1, 7, 5, &(GPS::readPositionData)),
  sensorInfo("GPS Aux",                    -1, -1, 8, 8, &(GPS::readAuxilliaryData)),
  sensorInfo("Barometer",                  -1, -1, 8, 6, &(Barometer::readAltitudeData)),
  sensorInfo("Load Cell Engine Left",      -1, -1, 9,  5, NULL),
  sensorInfo("Load Cell Engine Right",     -1, -1, 10, 5, NULL)
};

//   name,    board id (depracated; no longer used),  sensor id,  overall id (unique) , clock freq, func
sensorInfo sensor = sensorInfo("",0,0,0,0, NULL);

valveInfo valve_ids[7] = {
  valveInfo("LOX 2 Way", 20),
  valveInfo("LOX 5 Way", 21),
  valveInfo("LOX GEMS", 22),
  valveInfo("Propane 2 Way", 23),
  valveInfo("Propane 5 Way", 24),
  valveInfo("Propane GEMS", 25),
  valveInfo("High Pressure Solenoid", 26)
};
                        // name, id
valveInfo valve = valveInfo("",0);


/*
 *  Stores how often we should be requesting data from each sensor.
 */
int sensor_checks[sizeof(all_ids)/sizeof(sensorInfo)][2];

void setup() {
  Wire.begin();
  Serial.begin(9600);
  RFSerial.begin(57600);
  for (uint8_t i=0; i<sizeof(all_ids)/sizeof(sensorInfo); i++) {
    sensor_checks[i][0] = all_ids[i].clock_freq;
    sensor_checks[i][1] = 1;
  }
}


void loop() {

  String command = "";
  //  while (RFSerial.available()) {
  //    command += RFSerial.read();
  //  }
  command = "{26,1|456}";
  decode_received_packet(command);

  /*
  * Code for requesting data and relaying back to ground station
  */
  for (uint8_t j = 0; j < sizeof(all_ids)/sizeof(sensorInfo); j++) {
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
  return false;
}
