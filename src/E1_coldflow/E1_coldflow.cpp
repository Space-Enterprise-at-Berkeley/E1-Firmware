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

#define SERIAL_INPUT 1

#define NO_ADC 0

#if SERIAL_INPUT
  #define RFSerial Serial
#else
  #define RFSerial Serial6
#endif

// within loop state variables

uint8_t val_index = 0;
char command[50]; //input command from GS

/*
    Stores how often we should be requesting data from each sensor.
*/
int sensor_checks[numSensors][2];

valveInfo valve;
sensorInfo *sensor;

long startTime;
String packet;

void sensorReadFunc(int id);

void setup() {
  Wire.begin();
  Serial.begin(57600);
  RFSerial.begin(57600);

  while(!Serial);
  while(!RFSerial);

  debug("Setting up Config", DEBUG);
  config::setup();

  debug("Initializing Sensor Frequencies", DEBUG);

  for (int i = 0; i < numSensors; i++) {
    debug(String(i), DEBUG);
    debug("starting 1st line", DEBUG);
    sensor_checks[i][0] = sensors[i].clock_freq;
    debug("starting 2nd line", DEBUG);
    sensor_checks[i][1] = 1;
  }

  debug("Sensor IDs:", DEBUG);
  debug(String(sensors[0].name), DEBUG);

  // debug("Starting SD", DEBUG);

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

  // config::setup();

  debug("Initializing Libraries", DEBUG);

  Solenoids::init(LOX_2_PIN, LOX_5_PIN, LOX_GEMS_PIN, PROP_2_PIN, PROP_5_PIN, PROP_GEMS_PIN, HIGH_SOL_PIN);
  batteryMonitor::init(&Wire, batteryMonitorShuntR, batteryMonitorMaxExpectedCurrent);

  Ducers::init(numPressureTransducers, ptAdcIndices, ptAdcChannels, ptTypes, ads);

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
      Serial.print(command[i]);
      i++;
    }

    debug(String(command), DEBUG);
    int action = decode_received_packet(String(command), &valve, valves, numValves);
    if (action != -1) {
      take_action(&valve, action);
      packet = make_packet(valve.id, false);
      Serial.println(packet);
      #if SERIAL_INPUT != 1
        RFSerial.println(packet);
      #endif
      // write_to_SD(packet.c_str(), file_name);
    }
  }


   if (Automation::inStartup()) {
     debug("E-1 is in startup", DEBUG);
    if (Automation::checkStartupProgress()) {
      Automation::advanceStartup(farrbconvert.sensorReadings);
      packet = make_packet(29, false);
      Serial.println(packet);
      RFSerial.println(packet);
    }
  }

  if (Automation::inShutdown()) {
    if (Automation::checkShutdownProgress()) {
      Automation::advanceShutdown(farrbconvert.sensorReadings);
      packet = make_packet(29, false);
      Serial.println(packet);
      RFSerial.println(packet);
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
    sensor = &sensors[j];
    sensorReadFunc(sensor->id);
    packet = make_packet(sensor->id, false);
    Serial.println(packet);
    #if SERIAL_INPUT != 1
        RFSerial.println(packet);
      #endif
    // write_to_SD(packet.c_str(), file_name);
  }

  // For dashboard display
  delay(50); 

}


/**
 *
 */
void sensorReadFunc(int id) {
  switch (id) {
    case 0:
      #if NO_ADC == 1
        break;
      #else
        Thermocouple::Analog::readTemperatureData(farrbconvert.sensorReadings);
        farrbconvert.sensorReadings[1] = tempController::controlTemp(farrbconvert.sensorReadings[0]);
        farrbconvert.sensorReadings[2] = -1;
        break;
      #endif

    case 1:
      #if NO_ADC == 1
        break;
      #else
        Ducers::readAllPressures(farrbconvert.sensorReadings);
        break;
      #endif

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