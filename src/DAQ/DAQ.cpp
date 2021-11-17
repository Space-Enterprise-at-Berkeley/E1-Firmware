/*
   Main code for DAQ - vamshi
*/

#include "config.h"

#include <ducer.h>
#include <tempController.h>
#include <batteryMonitor.h>
#include <powerSupplyMonitor.h>
#include <Adafruit_NeoPixel.h>

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

#ifdef DAQ1
Thermocouple::Cryo _cryoTherms;
#elif DAQ2


Thermocouple::SPI_TC _cryoTherms;
unsigned long lastUpdate;
#endif

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

  #ifdef DAQ1
  debug("Initializing battery monitor");
  //batteryMonitor::init(&Wire1, batteryMonitorShuntR, batteryMonitorMaxExpectedCurrent, battMonINAAddr);
  debug("Initializing power supply monitors");
  powerSupplyMonitor::init(numPowerSupplyMonitors, powSupMonPointers, powSupMonAddrs, powerSupplyMonitorShuntR, powerSupplyMonitorMaxExpectedCurrent, &Wire1);

  debug("Initializing ducers");
  Ducers::init(numPressureTransducers, ptAdcIndices, ptAdcChannels, ptTypes, adsPointers);

  debug("Initializing Thermocouples");
  Thermocouple::Analog::init(numAnalogThermocouples, thermAdcIndices, thermAdcChannels, adsPointers);

  _cryoTherms = Thermocouple::Cryo();
  _cryoTherms.init(numCryoTherms, _cryo_boards, cryoThermAddrs, cryoTypes, &Wire, cryoReadsBackingStore);
  _cryoTherms.lowerI2CSpeed(&Wire);

  debug("Initializing Load Cell");
  LoadCell::init(loadcells, numLoadCells, lcDoutPins, lcSckPins, lcCalVals);
  #elif DAQ2
  _cryoTherms = Thermocouple::SPI_TC();
  _cryoTherms.init(numCryoTherms, cryoThermCS, cryoReadsBackingStore);
  lastUpdate = 0;

  pixels.begin();
  pixels.setBrightness(255);

  #endif
}

void loop() {

  //process command
  #ifdef ETH
  if (Udp.parsePacket()) {
    debug("received udp packet");
    IPAddress remote = Udp.remoteIP();
    for (int i=0; i < 4; i++) {
      Serial.print(remote[i], DEC);
      if (i < 3) {
        Serial.print(".");
      }
    }
    for (uint8_t i = 0; i < numGrounds; i++) {
    if(Udp.remoteIP() == groundIP[i]) {
      debug("received packet came from groundIP");
      receivedCommand = true;
      Udp.read(command, 75);
      debug(String(command));
      break;
    }
  }
  }
  #endif
  if (RFSerial.available() > 0) {
    int i = 0;
  
    while (RFSerial.available()) {
      command[i] = RFSerial.read();
      Serial.print(command[i]);
      i++;
    }
    receivedCommand = true;
  }
  
  if(receivedCommand) {
    debug(String(command));
    int8_t id = processCommand(String(command));
    if (id != -1 && id != -2) {
      packet = make_packet(id, false);
      Serial.println(packet);
      #ifndef SERIAL_INPUT_DEBUG
        RFSerial.println(packet);
      #endif
      #ifdef ETH
      sendEthPacket(packet.c_str());
      #endif
    }
    receivedCommand = false;
  }

  /*
     Code for requesting data and relaying back to ground station
  */
  #ifdef DAQ
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
  #elif DAQ2
  if(millis() - lastUpdate > updatePeriod){
    lastUpdate = millis();
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
  }
  switch(led_mode){
    case 0:
      baseColor(SAFE);
      break;
    case 1:
      baseColor(WORK);
      break;
    case 2:
      baseColor(COUNTDOWN);
      break;
    case 3:
      baseColor(COLORS);
      break;
    case 4:
      baseColor(ABORT);
      break;
    case 5:
      baseColor(CLEAR);
      break;
    case 6: 
      altCol(BG)
  }
  capFill(capLox, 0);
  capFill(capFuel, 1);
  #endif
  // delay(10);
  
}

#ifdef DAQ2
void baseColor(led_state state) {
  if (state == SAFE) {
       set_base(159, 43, 104);
  } else if (state == WORK) {
      set_base(255, 255, 255);
  } else if (state == COLORS) {
      rainbow(5);
      return;
  } else if (state == COUNTDOWN) {
      set_led(0, 0, 255, 0, NUM_LEDS);
  } else if (state == ABORT) {
      set_base(255, 0, 0);
  } else if (state == CLEAR) {
      pixels.clear();
  } else if (state == BG) {
      altCol(0, 20, 38, 253, 181, 21)
  }
  pixels.show();
}

void capFill(double cap, int side) {

    if (side == 0) {

        set_led(0, 0, 0, lox_bot, lox_top);

        double fillp = (cap - loxMin) / loxOff;
        int filln = (int) floor((lox_top - lox_bot) * fillp);
        int i = lox_bot;
        //i <= lox_bot + filln && 
        while (i <= lox_top) {
          if (i > lox_bot + filln && i <= lox_top) {
            pixels.setPixelColor(i, pixels.Color(102, 255, 102));
          } else {
            pixels.setPixelColor(i, pixels.Color(0, 30, 255));
          }
          i++;
        }
    } else if (side == 1) {

        set_led(0, 0, 0, fuel_bot, fuel_top);

        double fillp = (cap - fuelMin) / fuelOff;
        int filln = (int) floor((fuel_bot - fuel_top) * fillp);

        int i = fuel_bot;
        //i >= fuel_bot - filln && 
        while (i >= fuel_top) {
          if (i < fuel_bot - filln && i >= lox_top) {
            pixels.setPixelColor(i, pixels.Color(102, 255, 102));
          } else {
              pixels.setPixelColor(i, pixels.Color(255, 50, 0));
          }
            i--;
        }

     }
    pixels.show();
}

void set_led(int R, int G, int B, int mint, int maxt) {
    
    for (int i = mint; i <= maxt; i++) {
        pixels.setPixelColor(i, pixels.Color(R, G, B));
    }
}

void altCol(int R1, int G1, int B1, int R2, int G2, int B2) {
  
    for (int i = 0; i < NUM_LEDS; i++) {
        if ((i >= lox_bot && i <= lox_top) || (i >= fuel_top && i <= fuel_bot)) {
            continue;
        }
        if (i % 2  == 0) {
          pixels.setPixelColor(i, pixels.Color(R1, G1, B1));
        } else {
          pixels.setPixelColor(i, pixels.Color(R2, G2, B2));
        }
    }
}

void set_base(int R, int G, int B) {

    for (int i = 0; i < NUM_LEDS; i++) {
        if ((i >= lox_bot && i <= lox_top) || (i >= fuel_top && i <= fuel_bot)) {
            continue;
        }
        pixels.setPixelColor(i, pixels.Color(R, G, B));
    }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<pixels.numPixels(); i++) {
      if ((i >= lox_bot && i <= lox_top) || (i >= fuel_top && i <= fuel_bot)) {
            continue;
      }
      pixels.setPixelColor(i, Wheel((i*1+j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
    return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else {
    WheelPos -= 170;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
#endif


/**
 *
 */
void sensorReadFunc(int id) {
  switch (id) {
    case 1:
      debug("pressures all");
      Ducers::readAllPressures(farrbconvert.sensorReadings);
      break;
    // case 2:
    //   debug("battery stats");
    //   //batteryMonitor::readAllBatteryStats(farrbconvert.sensorReadings);
    //   break;
    case 3:
      debug("Load Cells");
      LoadCell::readLoadCells(farrbconvert.sensorReadings);
      break;
    case 4:
      debug("Cryo all");
      _cryoTherms.readCryoTemps(farrbconvert.sensorReadings);
      break;
    case 5:
      readPacketCounter(farrbconvert.sensorReadings);
      break;
    // case 6:
    //   powerSupplyMonitor::readAllBatteryStats(farrbconvert.sensorReadings);
    //   break;
    case 19:
      //Thermocouple::Analog::readSpecificTemperatureData(0, farrbconvert.sensorReadings);
      Thermocouple::Analog::readTemperatureData(farrbconvert.sensorReadings);
      break;
    default:
      Serial.println("some other sensor");
      break;
  }
}
