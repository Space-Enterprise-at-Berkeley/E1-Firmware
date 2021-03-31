#include "apogeeDetection.h"
#include <Arduino.h>
#include <batteryMonitor.h>
#include "Barometer.h"
#include "config.h"
#include "IMU.h"
#include "GPS.h"

#define SERIAL_INPUT 0

#if SERIAL_INPUT
  #define RFSerial Serial
#else
  #define RFSerial Serial6
#endif

#define GPS_Serial Serial8

uint8_t val_index = 0;
char command[75];

//Stores how often we should be requesting data from each sensor.
int sensor_checks[numSensors][2];

sensorInfo *sensor;

long startTime;
String packet;

void sensorReadFunc(int id);

IMU _imu;
Barometer bmp;
GPS gps(GPS_Serial);
ApogeeDetection detector;

void setup() {
  //Setting up Serial Connection
  Wire.begin();
  Serial.begin(57600);
  RFSerial.begin(57600);
  //Waiting for Serial Connection
  while(!Serial);
  while(!RFSerial);

  debug("Setting up Config");
  config::setup();

  debug("Initializing Sensor Frequencies");

  for (int i = 0; i < numSensors; i++) {
    sensor_checks[i][0] = sensors[i].clock_freq;
    sensor_checks[i][1] = 1;
  }

  debug("Starting SD");

  int res = sd.begin(SdioConfig(FIFO_SDIO));
  if (!res) {
    packet = make_packet(101, true);
    RFSerial.println(packet);
  }

  debug("Opening File");
  file.open(file_name, O_RDWR | O_CREAT);

  debug("Writing Dummy Data");
  // NEED TO DO THIS BEFORE ANY CALLS TO write_to_SD
  sdBuffer = new Queue();

  std::string start = "beginning writing data";
  if(!write_to_SD(start, file_name)) { // if unable to write to SD, send error packet
    packet = make_packet(101, true);
    RFSerial.println(packet);
  }

  // Initialization of Components
  debug("Initializing Libraries");
  batteryMonitor::init(&Wire, batteryMonitorShuntR, batteryMonitorMaxExpectedCurrent);
  _imu.init(&Wire);
  bmp.init(&Wire);
  gps.init();

  bmp.readAllData(farrbconvert.sensorReadings);
  double initAcc_z = farrbconvert.sensorReadings[2];
  _imu.readAccelerationData(farrbconvert.sensorReadings);
  double initAlt = farrbconvert.sensorReadings[0];

  detector.init(avgSampleRate, altVar, accVar, initAlt, initAcc_z);
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
    packet = make_packet(sensor->id, false);
    Serial.println(packet);

    #if SERIAL_INPUT != 1
        RFSerial.println(packet);
    #endif
    write_to_SD(packet.c_str(), file_name);
  }
  delay(10);
}

void sensorReadFunc(int id) {
  switch (id) {
    case 2:
      batteryMonitor::readAllBatteryStats(farrbconvert.sensorReadings);
      break;
    case 5:
      readPacketCounter(farrbconvert.sensorReadings);
      break;
    case 11:
      gps.readPositionData(farrbconvert.sensorReadings);
      break;
    case 12:
      gps.readAuxilliaryData(farrbconvert.sensorReadings);
      break;
    case 13:
      bmp.readAllData(farrbconvert.sensorReadings);
      detector.updateAlt(farrbconvert.sensorReadings);
      break;
    case 14:
      _imu.readAccelerationData(farrbconvert.sensorReadings);
      if(detector.atApogee()) {
        // deloy chute 
        // send recovery ack packet
      }
      detector.updateAcc(farrbconvert.sensorReadings);
      break;
    case 15:
      _imu.readOrientationData(farrbconvert.sensorReadings);
      break;
    default:
      Serial.println("some other sensor");
      break;
  }
}
