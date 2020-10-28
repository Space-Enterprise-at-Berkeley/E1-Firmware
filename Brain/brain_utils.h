/*
 * Brain_utils.h- A c++ program that uses I2C to establish communication between 
 * the sensors and valves inside to the rocket with the ground station. Able to send
 * data to the ground station via RF. Can receive and process commands sent from 
 * ground station. This contains additional functions and structs used in Brain_I2C.ino.
 * Created by Vainavi Viswanath, Aug 21, 2020.
 */
#include "solenoids.h"

String make_packet(struct sensorInfo sensor);
uint16_t Fletcher16(uint8_t *data, int count);
void chooseValveById(int id, struct valveInfo *valve);

/*
 * Data structure to allow the conversion of bytes to floats and vice versa.
 */
union floatArrToBytes {
  char buffer[24];
  float sensorReadings[6];
} farrbconvert;

/*
 * Data structure to store all information relevant to a specific sensor type.
 */
struct sensorInfo {
  String name;
  int board_address;
  int id;
  int clock_freq;
};

/*
 * Data structure to store all information relevant to a specific valve.
 */
struct valveInfo {
  String name;
  int id;
  int (*openValve)();
  int (*closeValve)();
};

const int numValves = 9;

valveInfo valves[9] = {
  {"LOX 2 Way", 20, &(Solenoids::armLOX), &(Solenoids::disarmLOX)}, //example
  {"LOX 5 Way", 21, &(Solenoids::openLOX), &(Solenoids::closeLOX)},
  {"LOX GEMS", 22, &(Solenoids::ventLOXGems), &(Solenoids::closeLOXGems)},
  {"Propane 2 Way", 23, &(Solenoids::armPropane), &(Solenoids::disarmPropane)},
  {"Propane 5 Way", 24, &(Solenoids::openPropane), &(Solenoids::closePropane)},
  {"Propane GEMS", 25, &(Solenoids::ventPropaneGems), &(Solenoids::closePropaneGems)},
  {"High Pressure Solenoid", 26, &(Solenoids::activateHighPressureSolenoid), &(Solenoids::deactivateHighPressureSolenoid)},
  {"Arm Rocket", 27, &(Solenoids::armAll), &(Solenoids::disarmAll)},
  {"Launch Rocket", 28, &(Solenoids::LAUNCH), &(Solenoids::endBurn)}
};


/*
 * Constructs packet in the following format: 
 * {<sensor_ID>,<data1>,<data2>, ...,<dataN>|checksum}
 */
String make_packet(struct sensorInfo sensor) {
  String packet_content = (String)sensor.id;
  packet_content += ",";
  for (int i=0; i<6; i++) {
    float reading = farrbconvert.sensorReadings[i];
    if (reading != -1) {
      packet_content += (String)reading;
      packet_content += ",";
    } else {
      break;
    }
  }
  packet_content.remove(packet_content.length()-1); 
  int count = packet_content.length();
  char const *data = packet_content.c_str();
  uint16_t checksum = Fletcher16((uint8_t *) data, count);
  packet_content += "|";
  packet_content += String(checksum, HEX);
  String packet = "{" + packet_content + "}";
  
  return packet;
}

/*
 * Decodes a packet sent from ground station in the following format:
 * {<valve_ID>,<open(1) or close(0)|checksum}
 * Populated the fields of the valve and returns the action to be taken
 */
int decode_received_packet(String packet, valveInfo *valve) {
  int ind1 = packet.indexOf(',');
  int valve_id = packet.substring(1,ind1).toInt();
  int ind2 = packet.indexOf('|');
  int action = packet.substring(ind1+1,ind2).toInt();
  Serial.println(action);
  Serial.flush();
  String checksumstr = packet.substring(ind2+1, packet.length()-2);
  char checksum_char[5];
  checksumstr.toCharArray(checksum_char, 5);
  uint16_t checksum = strtol(checksum_char, NULL, 16); //checksumstr.toInt();//std::stoi(checksumstr, 0, 16);
  Serial.println(checksum);
  Serial.flush();
  char const *data = packet.substring(1,ind2).c_str();
  int count = packet.substring(1,ind2).length();
  uint16_t check = Fletcher16((uint8_t *) data, count);
  if (check == checksum) {
    Serial.println("got valid command");
    Serial.println(valve_id);
    Serial.println(action);
    Serial.println(checksum);
    Serial.flush();
//    chooseValveById(valve_id, &valve);
    for (int i = 0; i < numValves; i++) {
      Serial.println(valves[i].id);
      Serial.flush();
      if (valves[i].id == valve_id) {
        valve = &valves[i];
        Serial.println("id: ");
        Serial.println(valve->id);
        break;
      }
    }
    Serial.println("finished choose valve");
    Serial.println(valve->id);
    return action;
  } else {
    return -1;
  }
}

void chooseValveById(int id, valveInfo *valve) {
  Serial.println("called choose valve");
  Serial.flush();
  for (int i = 0; i < numValves; i++) {
    Serial.println(valves[i].id);
    Serial.flush();
    if (valves[i].id == id) {
      valve = &valves[i];
      Serial.println("id: ");
      Serial.println(valve->id);
      break;
    }
  }
}


/*
 * Calls the corresponding method for this valve with the appropriate
 * action in solenoids.h
 */
void take_action(valveInfo *valve, int action) {
  Serial.println("called take action");
  Serial.flush();
  if (action) {
    Serial.println("opening valve");
    Serial.flush();
    valve->openValve();
    Serial.println("done");
    Serial.flush();
  } else {
    Serial.println("closing valve");
    valve->closeValve();
    Serial.println("done");
    Serial.flush();
  }
  Serial.println("finished command");
}

void take_action_2 (int id, int action) {
  Serial.println("called take action 2");
  Serial.println(id);
  Serial.println(action);
  switch(id) {
    case 20:
      if(action) {
        Solenoids::armLOX();
      } else {
        Solenoids::disarmLOX();
      }
      break;
    case 21:
      Serial.println("lox 2 way");
      Serial.flush();
      if(action == 1) {
        Solenoids::openLOX();
      } else {
        Solenoids::closeLOX();
      }
      break;
    case 22:
      if(action){
        Solenoids::ventLOXGems();
      } else {
        Solenoids::closeLOXGems();
      }
      break;
    case 23:
      if(action){
        Solenoids::armPropane();
      } else {
        Solenoids::disarmPropane();
      }
      break;
    case 24:
      if(action){
        Solenoids::openPropane();
      } else {
        Solenoids::closePropane();
      }
      break;
  }
  
}

/*
 * Calculates checksum for key values being sent to ground station:
 * sensor_ID and it's corresponding data points
 */
uint16_t Fletcher16(uint8_t *data, int count) {
  
  uint16_t sum1 = 0;
  uint16_t sum2 = 0;

  for (int index=0; index<count; index++) {
    if (data[index] > 0) {
      sum1 = (sum1 + data[index]) % 255;
      sum2 = (sum2 + sum1) % 255;
    }
  }
  return (sum2 << 8) | sum1;
}
