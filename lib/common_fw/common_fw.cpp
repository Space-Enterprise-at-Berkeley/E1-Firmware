/*
 * common_fw.cpp- A c++ program that houses the firmware that is reasonably hardware
 * agnostic and can be repeated across different boards and flight hardware.
 * Created by Vainavi Viswanath, Aug 21, 2020.
 */
#include "common_fw.h"

#define PACKET_START 0x2A56
#define PACKET_END 0xC51F

SdFat sd;
File file;
struct Queue *sdBuffer;
union floatArrToBytes farrbconvert;
char packet[64];
char command[64];

/**
 *
 */
bool write_to_SD(std::string message, const char * file_name) {
    sdBuffer->enqueue(message);
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
      } else {        //If the file didn't open
        return false;
      }
    }
    return true;
}

/*
 * Constructs packet in the following format:
 * {<sensor_ID>,<data1>,<data2>, ...,<dataN>|checksum}
 */
uint8_t make_packet(uint8_t id, bool error) {
  int i = 0;
  packet[i++] = (PACKET_START >> 8) & 0xFF;
  packet[i++] = PACKET_START & 0xFF;

  packet[i++] = id;
  if (!error) {
    for (int j=0; j<7; j++) {
      if (farrbconvert.sensorReadings[j] != -1) {
        std::copy(farrbconvert.buffer + j * 4,
                  farrbconvert.buffer + (j+1) * 4,
                  packet + i);
        i += 4;
      } else {
        break;
      }
    }
  } else {
    packet[i++] = 0;
  }

  uint16_t checksum = Fletcher16(packet+1, i-1);
  packet[i++] = (checksum >> 8) & 0xFF;
  packet[i++] = checksum & 0xFF;

  packet[i++] = (PACKET_END >> 8) & 0xFF;
  packet[i++] = PACKET_END & 0xFF;
  return i;
}

/*
 * Decodes a packet sent from ground station in the following format:
 * {<valve_ID>,<open(1) or close(0)|checksum}
 * Populated the fields of the valve and returns the action to be taken
 * This is a pretty beefy function; can we split this up
 */
int8_t decode_received_packet(uint8_t *_command, valveInfo *valve, valveInfo valves[], int numValves) {
  uint8_t i = 0;
  uint16_t command_open = (_command[i++] << 8) & _command[i++];
  if (command_open != PACKET_START) {
    return -1;
  }
  int valve_id = _command[i++];

  int action = _command[i++]; // currently, action is assumed to be a single int.
  uint16_t checksum = (_command[i++] << 8) & _command[i++];
  uint16_t _check = Fletcher16(_command+2, 2); // just id and action both of which are ints.

  int size = sprintf(packet,"%x",checksum);
  debug(packet, size);
  size = sprintf(packet,"%x", _check);
  debug(packet, size);

  if (_check != checksum) {
    return -1;
  }

  if((uint16_t)((_command[i++] << 8) & _command[i++]) != PACKET_END){
    return -1;
  }

  debug("Checksum correct, taking action");
  chooseValveById(valve_id, valve, valves, numValves);
  return action;
}

/**
 *
 */
void chooseValveById(int id, valveInfo *valve, valveInfo valves[], int numValves) {
  for (int i = 0; i < numValves; i++) {
    if (valves[i].id == id) {
      *valve = valves[i];
      break;
    }
  }
}

/*
 * Calls the corresponding method for this valve with the appropriate
 * action in solenoids.h
 */
void take_action(valveInfo *valve, int action) {
  if (action == 1) {
    valve->openValve();
  } else if (action == 0) {
    valve->closeValve();
  }
  if(action != -1)
    valve->ackFunc(farrbconvert.sensorReadings);
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

void debug(std::string str){
  #ifdef DEBUG
    Serial.write(str.c_str());
    Serial.flush();
  #endif
}

void debug(uint8_t *bytes, int count){
  #ifdef DEBUG
    Serial.write(bytes, count);
    Serial.flush();
  #endif
}
