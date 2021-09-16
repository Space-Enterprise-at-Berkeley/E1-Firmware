/*
   Main code for DAQ - vamshi
*/

#include "config2.h"

#define RFSerial Serial

// within loop state variables

uint8_t val_index = 0;
char command[75]; //input command from GS

/*
    Stores how often we should be requesting data from each sensor.
*/
int sensor_checks[numSensors][2];

sensorInfo *sensor;

long startTime;
String packet;

void sensorReadFunc(int id);

Thermocouple::SPI _cryoTherms;

void setup() {
  Wire.begin();
  Serial.begin(57600);

  delay(3000);

  #ifdef ETH
  debug("Setup Ethernet");
  setupEthernetComms(mac, ip);
  #endif

  debug("Setting up Config");
  config::setup();

  debug("Initializing Sensor Frequencies");

  for (int i = 0; i < numSensors; i++) {
    sensor_checks[i][0] = sensors[i].clock_freq;
    sensor_checks[i][1] = 1;
  }

  debug("Initializing Libraries");

  debug("Initializing Thermocouples");

  _cryoTherms = Thermocouple::SPI();
  _cryoTherms.init(numCryoTherms, _cryo_boards, cryoThermCS, cryoThermCLK, cryoThermDO, cryoReadsBackingStore);
}

void loop() {

  // process command
  // #ifdef ETH
  // if (Udp.parsePacket()) {
  //   debug("received udp packet");
  //   IPAddress remote = Udp.remoteIP();
  //   for (int i=0; i < 4; i++) {
  //     Serial.print(remote[i], DEC);
  //     if (i < 3) {
  //       Serial.print(".");
  //     }
  //   }
  //   for (uint8_t i = 0; i < numGrounds; i++) {
    // if(Udp.remoteIP() == groundIP[i]) {
      // debug("received packet came from groundIP");
      // receivedCommand = true;
      // Udp.read(command, 75);
      // debug(String(command));
      // break;
    // }
  // }
  // }
  // #endif
  // if (RFSerial.available() > 0) {
  //   int i = 0;
  //
  //   while (RFSerial.available()) {
  //     command[i] = RFSerial.read();
  //     Serial.print(command[i]);
  //     i++;
  //   }
  //   receivedCommand = true;
  // }
  //
  // if(receivedCommand) {
  //   debug(String(command));
  //   int8_t id = processCommand(String(command));
  //   if (id != -1) {
  //     packet = make_packet(id, false);
  //     Serial.println(packet);
  //     #ifndef SERIAL_INPUT_DEBUG
  //       RFSerial.println(packet);
  //     #endif
  //     #ifdef ETH
  //     sendEthPacket(packet.c_str());
  //     #endif
  //     write_to_SD(packet.c_str(), file_name);
  //   }
  //   receivedCommand = false;
  // }

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
    #ifdef ETH
    sendEthPacket(packet.c_str());
    #endif
    #ifndef SERIAL_INPUT_DEBUG
        RFSerial.println(packet);
    #endif

    //write_to_SD(packet.c_str(), file_name);
  }
  // delay(10);
}


/**
 *
 */
void sensorReadFunc(int id) {
  switch (id) {
    case 4:
      debug("Cryo all");
      _cryoTherms.readCryoTemps(farrbconvert.sensorReadings);
      break;
    case 5:
      readPacketCounter(farrbconvert.sensorReadings);
      break;
    default:
      Serial.println("some other sensor");
      break;
  }
}
