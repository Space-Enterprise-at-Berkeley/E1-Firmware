/*
   Brain_I2C.ino - A c++ program that uses I2C to establish communication between
   the sensors and valves inside to the rocket with the ground station. Able to send
   data to the ground station via RF. Can receive and process commands sent from
   ground station.
   Created by Vainavi Viswanath, Aug 21, 2020.
*/

#include "config.h"

#include <ducer.h>
#include <tempController.h>
#include <batteryMonitor.h>

#define SERIAL_INPUT 0

#if SERIAL_INPUT
  #define RFSerial Serial
#else
  #define RFSerial Serial6
#endif


uint8_t val_index = 0;
uint8_t packet_size;

using namespace std;
/*
    Stores how often we should be requesting data from each sensor.
*/
int sensor_checks[numSensors][2];

valveInfo valve;
sensorInfo *sensor;

long startTime;

void sensorReadFunc(int id);

void setup() {
  Wire.begin();
  Serial.begin(57600);
  RFSerial.begin(57600);

  while(!Serial);
  while(!RFSerial);

  debug("Setting up Config");
  config::setup();

  debug("Initializing Sensor Frequencies");

  for (int i = 0; i < numSensors; i++) {
    int size = sprintf(packet, "%d", i);
    debug(packet, size);
    debug("starting 1st line");
    sensor_checks[i][0] = sensors[i].clock_freq;
    debug("starting 2nd line");
    sensor_checks[i][1] = 1;
  }

  debug("Sensor IDs:");
  debug(sensors[0].name);

  debug("Starting SD");

  int res = sd.begin(SdioConfig(FIFO_SDIO));
  if (!res) {
    packet_size = make_packet(101, true);
    RFSerial.write(packet, packet_size);
  }

  debug("Opening File");
  file.open(file_name, O_RDWR | O_CREAT);
  file.close();

  debug("Writing Dummy Data");
  sdBuffer = new Queue();

  std::string start = "beginning writing data";
  if(!write_to_SD(start, file_name)) { // if unable to write to SD, send error packet
    packet_size = make_packet(101, true);
    RFSerial.write(packet, packet_size);
  }

  debug("Initializing Libraries");

  Solenoids::init(LOX_2_PIN, LOX_5_PIN, LOX_GEMS_PIN, PROP_2_PIN, PROP_5_PIN, PROP_GEMS_PIN, HIGH_SOL_PIN);
  batteryMonitor::init(&Wire, batteryMonitorShuntR, batteryMonitorMaxExpectedCurrent);

  Ducers::init(numPressureTransducers, ptAdcIndices, ptAdcChannels, ads);

  Thermocouple::Analog::init(numAnalogThermocouples, thermAdcIndices, thermAdcChannels, ads);
  Thermocouple::Cryo::init(numCryoTherms, cryoThermAddrs, cryoTypes);

  tempController::init(10, 2, LOX_ADAPTER_HEATER_PIN); // setPoint = 10 C, alg = PID, heaterPin = 7
}

void loop() {
  // process command
  if (RFSerial.available() > 0) {
    int i = 0;
    while (RFSerial.available()) {
      command[i] = RFSerial.read();
      if (i == 0) {
        if (command[i] != (PACKET_START >> 8) & 0xFF) {
          continue;
        } //if first two bytes are not start packet, scrap packet.
      } else if (i == 1) {
        if (command[i] != PACKET_START & 0xFF){
          i = 0;
          continue;
        }
      } else { // stop recording once we see end packet
        if (command[i] == PACKET_END & 0xFF &&
            command[i-1] == (PACKET_END >> 8) & 0xFF){
              ++i;
              break;
            }
      }
      i++;
    }
    debug(command, i);
    if (i > 4) { // startpacket + end packet is 4 bytes.
      int action = decode_received_packet(command, &valve, valves, numValves);
      if (action != -1) {
        take_action(&valve, action);
        packet_size = make_packet(valve.id, false);
        Serial.write(packet, packet_size);
        #if SERIAL_INPUT != 1
          RFSerial.write(packet, packet_size);
        #endif
        packet[packet_size] = '\0';
        write_to_SD(packet, file_name);
      }
    }
  }

  //Code for requesting data and relaying back to ground station
  for (int j = 0; j < numSensors; j++) {
    if (sensor_checks[j][0] == sensor_checks[j][1]) {
      sensor_checks[j][1] = 1;
    } else {
      sensor_checks[j][1] += 1;
      continue;
    }
    sensor = &sensors[j];
    sensorReadFunc(sensor->id);
    packet_size = make_packet(sensor->id, false);
    Serial.write(packet, packet_size);
    #if SERIAL_INPUT != 1
        RFSerial.write(packet, packet_size);
    #endif
    packet[packet_size] = '\0';
    write_to_SD(packet, file_name);
  }
  // delay(100);
}


/**
 *
 */
void sensorReadFunc(int id) {
  switch (id) {
    case 0:
      Thermocouple::Analog::readTemperatureData(farrbconvert.sensorReadings);
      farrbconvert.sensorReadings[1] = tempController::controlTemp(farrbconvert.sensorReadings[0]);
      farrbconvert.sensorReadings[2] = -1;
      break;
    case 1:
      Ducers::readAllPressures(farrbconvert.sensorReadings);
      break;
    case 2:
      batteryMonitor::readAllBatteryStats(farrbconvert.sensorReadings);
      break;
    case 4:
      Thermocouple::Cryo::readCryoTemps(farrbconvert.sensorReadings);
      //farrbconvert.sensorReadings[1]=0;
      farrbconvert.sensorReadings[2]=0;
      farrbconvert.sensorReadings[3]=0;
      farrbconvert.sensorReadings[4]=-1;
      break;
    default:
      Serial.println("some other sensor");
      break;
  }
}
