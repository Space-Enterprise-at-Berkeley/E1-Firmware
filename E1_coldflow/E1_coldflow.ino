/*
   Brain_I2C.ino - A c++ program that uses I2C to establish communication between
   the sensors and valves inside to the rocket with the ground station. Able to send
   data to the ground station via RF. Can receive and process commands sent from
   ground station.
   Created by Vainavi Viswanath, Aug 21, 2020.
*/
//
#include <Wire.h>
#include "brain_utils.h"
#include <GPS.h>
#include <Barometer.h>
#include <ducer.h>

#define RFSerial Serial6
#define GPSSerial Serial8

#define FLIGHT_BRAIN_ADDR 0x00

// within loop state variables
uint8_t board_address = 0;
uint8_t sensor_id = 0;
uint8_t val_index = 0;
char command[50]; //input command from GS

const uint8_t numSensors = 8;

/*
   Array of all sensors we would like to get data from.
*/
sensorInfo sensors[numSensors] = {
  {"Temperature",                FLIGHT_BRAIN_ADDR, 0, 3}, //&(testTempRead)}, //&(Thermocouple::readTemperatureData)},
  {"All Pressure",               FLIGHT_BRAIN_ADDR, 1, 1},
  {"Battery Stats",              FLIGHT_BRAIN_ADDR, 2, 3},
  {"Aux temp",                   FLIGHT_BRAIN_ADDR, 4, 1},
};

/*
    Stores how often we should be requesting data from each sensor.
*/
int sensor_checks[numSensors][2];

valveInfo valve;
sensorInfo sensor = {"temp", 23, 23, 23};

std::string str_file_name = "E1_speed_test_results.txt";
const char * file_name = str_file_name.c_str();

long startTime;
String packet;

void setup() {
  Wire.begin();
  Serial.begin(57600);
  RFSerial.begin(57600);
  GPSSerial.begin(4608000);

  delay(5000);
  Serial.println("Initializing Sensors & Libraries");
  Serial.flush();


  delay(1000);

  Serial.println("Initializing Sensors Arrays");
  Serial.flush();
  for (int i = 0; i < numSensors; i++) {
    sensor_checks[i][0] = sensors[i].clock_freq;
    sensor_checks[i][1] = 1;
  }

  Serial.println("Configuring SD");
  Serial.flush();
  int res = sd.begin(SdioConfig(FIFO_SDIO));
  if (!res) {
    packet = make_packet(101, true);
    RFSerial.println(packet);
  }
  Serial.println("Opening File");
  Serial.flush();
  file.open(file_name, O_RDWR | O_CREAT);
  Serial.println("Closing File");
  Serial.flush();
  file.close();
  Serial.println("Writing Dummy Data");
  Serial.flush();
  std::string start = "beginning writing data";
  Serial.println("Creating Queue");
  Serial.flush();
  sdBuffer = new Queue();
  if(!write_to_SD(start)) { // if unable to write to SD, send error packet
    Serial.println("Reporting SD Error");
    Serial.flush();
    packet = make_packet(101, true);
    RFSerial.println(packet);
  }

  Serial.println("Initializing ADCs");
  Serial.flush();
  // initialize all ADCs
  //ads = (ADS1219 **) malloc(numADCSensors * sizeof(ADS1219));
  ads = new ADS1219*[numADCSensors];
  for (int i = 0; i < numADCSensors; i++) {
    Serial.print("Initializing ADC ");
    Serial.println(i);
    Serial.flush();
    ADS1219 *a = new ADS1219(adcDataReadyPins[i], ADSAddrs[i], &Wire);
    ads[i] = a;
    ads[i]->setConversionMode(SINGLE_SHOT);
    ads[i]->setVoltageReference(REF_EXTERNAL);
    ads[i]->setGain(GAIN_ONE);
    ads[i]->setDataRate(90);
    pinMode(adcDataReadyPins[i], INPUT_PULLUP);
    Serial.println("Calibrating ADC");
    Serial.flush();
//    Serial.println(ads[i]->readData(0));
//    Serial.println(ads[i]->readData(1));
//    Serial.println(ads[i]->readData(2));
//    Serial.println(ads[i]->readData(3));
//    ads[i]->calibrate();
  }

  Serial.println("Initializing Recovery");
  Serial.flush();
  Recovery::init();
  Serial.println("Initializing Solenoids");
  Serial.flush();
  Solenoids::init();
  Serial.println("Initializing Battery Monitor");
  Serial.flush();
  batteryMonitor::init();

  Ducers::init(numPressureTransducers, ptAdcIndices, ptAdcChannels, ads);
  Thermocouple::Analog::init(numAnalogThermocouples, thermAdcIndices, thermAdcChannels, ads);
  Thermocouple::Cryo::init(numCryoTherms, cryoThermAddrs, cryoTypes);

  tempController::init(10, 2, 7); // setPoint = 10 C, alg = PID, heaterPin = 7

  Serial.println(ads[0]->readData(0));
  Serial.print("ADC 0 pointer: ");
  Serial.println((int32_t) ads[0]);
  Serial.print("ADC 1 pointer: ");
  Serial.println((int32_t) ads[1]);
}

void loop() {
  Serial.println("Looping");
  Serial.flush();
  delay(1000);

//  if (RFSerial.available() > 0) {
//    Serial.println("Decoding Message");
//    int i = 0;
//    while (RFSerial.available()) {
//      command[i] = RFSerial.read();
//      Serial.print(command[i]);
//      i++;
//    }
//    Serial.println();
//    Serial.println(String(command));
//    int action = decode_received_packet(String(command), &valve);
//    if (action != -1) {
//      take_action(&valve, action);
//      packet = make_packet(valve.id, false);
//      Serial.println(packet);
//      RFSerial.println(packet);
//      write_to_SD(packet.c_str());
//    }
//  }
//
  Serial.println("??");
  Serial.flush();
  /*
     Code for requesting data and relaying back to ground station
  */
  for (int j = 0; j < numSensors; j++) {
    Serial.print(" loop ");
    Serial.print(j);
    Serial.flush();
    if (sensor_checks[j][0] == sensor_checks[j][1]) {
      sensor_checks[j][1] = 1;
    } else {
      sensor_checks[j][1] += 1;
      continue;
    }
    Serial.println("reading sensor");
    Serial.flush();
//    Serial.println(ads[0]->readData(0));
    sensor = sensors[j];
    board_address = sensor.board_address;
    sensor_id = sensor.id;
    sensorReadFunc(sensor.id);
    packet = make_packet(sensor.id, false);
    Serial.println(packet);
    RFSerial.println(packet);
    write_to_SD(packet.c_str());
  }
}

bool write_to_SD(std::string message) {
  // every reading that we get from sensors should be written to sd and saved.

    Serial.println("Writing to SD");
    Serial.flush();
    sdBuffer->enqueue(message);
    Serial.println("Checking Queue Length");
    Serial.flush();
    if(sdBuffer->length >= 40) {
      if(file.open(file_name, O_RDWR | O_APPEND)) {
        int initialLength = sdBuffer->length;
        for(int i = 0; i < initialLength; i++) {
          char *msg = sdBuffer->dequeue();
          file.write(msg);
          free(msg);
        }
        file.close();
        return true;
      } else {                                                            //If the file didn't open
        return false;
      }
    }
    return true;
}
