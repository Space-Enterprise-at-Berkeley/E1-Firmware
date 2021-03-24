/*
 * common_fw.h- A c++ program that houses the firmware that is reasonably hardware
 * agnostic and can be repeated across different boards and flight hardware.
 * Created by Vainavi Viswanath, Aug 21, 2020.
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <string>
#include <cstring>

// Arduino Libraries
#include <SPI.h>
#include <SdFat.h>
#include <TimeLib.h>
#include <Arduino.h>
#include <Wire.h>

#include <actuator.h>

const uint8_t qMaxSize = 40;

struct Queue {

  struct Node {
    char message[75];
    int length;
  };

  uint16_t length = 0;
  uint8_t front = 0, end = 0;

  struct Node q[qMaxSize];

  Queue() {}

  void enqueue(std::string message) {
    q[end].length = message.length();

    strncpy(q[end].message, message.c_str(), message.length());
    q[end].message[q[end].length] = '\n'; // add \n to string when enqueue

    length++;
    end++;
    if (end == qMaxSize) {
      end = 0;
    }
  }

  uint8_t dequeue(char * buffer) {
    if(length > 0) {
      #ifdef DEBUG
          Serial.println("dequeuing, curr size " + String(length));
          Serial.flush();
      #endif
      length--;

      strncpy(buffer, q[front].message, q[front].length + 1);
      uint8_t strLength = q[front].length + 1;
      front++;
      if (front == qMaxSize) {
        front = 0;
      }
      return strLength;
    }
    #if DEBUG
      Serial.println("queue is empty. returning null pointer");
      Serial.flush();
    #endif
    return -1;
  }
};

const uint8_t maxReadings = 9;

/*
 * Data structure to allow the conversion of bytes to floats and vice versa.
 */
union floatArrToBytes {
  char buffer[4 * maxReadings];
  float sensorReadings[maxReadings];
};

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
// struct valveInfo {
//   String name;
//   int id;
//   int (*openValve)();
//   int (*closeValve)();
//   void (*ackFunc)(float *data);
// };



String make_packet (int id, bool error);
uint16_t Fletcher16 (uint8_t *data, int count);
// void chooseValveById (int id, struct valveInfo *valve, valveInfo valves[], int numValves);
bool write_to_SD(std::string message, const char * file_name);
int8_t parseCommand(String packet);
void readPacketCounter(float *data);
void incrementPacketCounter();
// void take_action(valveInfo *valve, int action);
uint16_t Fletcher16(uint8_t *data, int count);
void debug(String str);

extern SdFat sd;
extern File file;

extern struct Queue *sdBuffer;

extern union floatArrToBytes farrbconvert;
extern struct sensorInfo *sensors;
extern const uint8_t numActuators;
extern ActuatorArray actuators;
extern int packetCounter;
#endif // _COMMON_H_
