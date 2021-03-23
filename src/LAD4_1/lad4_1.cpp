
#include <Arduino.h>
#include "config.h"
#include <batteryMonitor.h>
#include "IMU.h"
#include "Barometer.h"
#include "GPS.h"


#define SERIAL_INPUT 0

#if SERIAL_INPUT
  #define RFSerial Serial
#else
  #define RFSerial Serial6
#endif

#define GPS_Serial Serial8

uint8_t val_index = 0;
char command[50];

//Stores how often we should be requesting data from each sensor.
int sensor_checks[numSensors][2];

sensorInfo *sensor;

long startTime;
String packet;

void sensorReadFunc(int id);

IMU _imu;
Barometer bmp;
GPS gps = GPS(GPS_Serial);

void setup() {
  //Setting up Serial Connection
  Wire.begin();
  Serial.begin(57600);
  RFSerial.begin(57600);
  //Waiting for Serial Connection
  while(!Serial);
  while(!RFSerial);

  debug("Setting up Config", DEBUG);
  config::setup();

  debug("Initializing Sensor Frequencies", DEBUG);

  for (int i = 0; i < numSensors; i++) {
    sensor_checks[i][0] = sensors[i].clock_freq;
    sensor_checks[i][1] = 1;
  }

  debug("Starting SD", DEBUG);

  int res = sd.begin(SdioConfig(FIFO_SDIO));
  if (!res) {
    packet = make_packet(101, true);
    RFSerial.println(packet);
  }

  debug("Opening File", DEBUG);
  file.open(file_name, O_RDWR | O_CREAT);
  file.close();

  debug("Writing Dummy Data", DEBUG);
  // NEED TO DO THIS BEFORE ANY CALLS TO write_to_SD
  sdBuffer = new Queue();

  std::string start = "beginning writing data";
  if(!write_to_SD(start, file_name)) { // if unable to write to SD, send error packet
    packet = make_packet(101, true);
    RFSerial.println(packet);
  }

  debug("Initializing Libraries", DEBUG);

  batteryMonitor::init(&Wire, batteryMonitorShuntR, batteryMonitorMaxExpectedCurrent);

  _imu = IMU();
  _imu.init(&Wire);

  bmp = Barometer();
  bmp.init(&Wire);

  gps.init();

  // Initializes initial state _x in Kalman filter to the alt and acc of the rocket at setup
  sensorReadFunc(0);
  double acc_z = farrbconvert.sensorReadings[2];
  sensorReadFunc(4)
  double altitude = farrbconvert.sensorReadings[0];

  double altVar = 0.5;
  double accVar = 0.5;
  ApogeeDetection(20e-3, altVar, accVar, altitude, acc_z);

}

void loop() {
  // process command
  if (RFSerial.available() > 0) {
    int i = 0;

    while (RFSerial.available()) {
      command[i] = RFSerial.read();
      Serial.print(command[i]);
      i++;
    }

  }

  //check for apogee somewhere in this loop, probably by storing altitude and acceleration data when
  //id is 0 (imu acceleration data) and 2 (barometer altitude), and passing those values into the detection function
  //that is initialized above
  for (int j = 0; j < numSensors; j++) {
    if (sensor_checks[j][0] == sensor_checks[j][1]) {
      sensor_checks[j][1] = 1;
    } else {
      sensor_checks[j][1] += 1;
      continue;
    }
    sensor = &sensors[j];
    sensorReadFunc(sensor->id);
    if(sensor->id == 0) {
      acc_z = data[0]
    }
    else if(sensor->id == 2) {
      altitude = data[2];
      if(atApogee(altitude, acc_z)) {
        //deloy chute 
      }
    }
    packet = make_packet(sensor->id, false);
    Serial.println(packet);

    #if SERIAL_INPUT != 1
        RFSerial.println(packet);
    #endif
    write_to_SD(packet.c_str(), file_name);
  }

}

void sensorReadFunc(int id) {
  switch (id) {
    case 0:
      _imu.readAccelerationData(farrbconvert.sensorReadings);
      break;
    case 1:
      _imu.readOrientationData(farrbconvert.sensorReadings);
      break;
    case 2:
      bmp.readAllData(farrbconvert.sensorReadings);
      break;
    case 3:
      batteryMonitor::readAllBatteryStats(farrbconvert.sensorReadings);
      break;
    case 4:
      gps.readPositionData(farrbconvert.sensorReadings);
      break;
    case 5:
      gps.readAuxilliaryData(farrbconvert.sensorReadings);
      break;
    case 6:
      readPacketCounter(farrbconvert.sensorReadings);
      break;
    default:
      Serial.println("some other sensor");
      break;
  }
}
