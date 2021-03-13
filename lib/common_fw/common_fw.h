/*
 * common_fw.h- A c++ program that houses the firmware that is reasonably hardware
 * agnostic and can be repeated across different boards and flight hardware.
 * Created by Vainavi Viswanath, Aug 21, 2020.
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <string>
#include <cstring>
#include <SPI.h>
#include <SdFat.h>
#include <TimeLib.h>
#include <Arduino.h>
#include <Wire.h>

#include <actuator.h>


//struct Queue {
  //
  // struct Node {
  //   struct Node *next;
  //   char *message;
  //   int length; //length doesn't include the null terminator
  // };
  //
  // uint16_t length = 0;
  //
  // struct Node *end = 0;
  // struct Node *front = 0;
  //
  // Queue() {
  //   length = 0;
  //   end = nullptr;
  //   front = nullptr;
  // }
  //
  // void enqueue(std::string message) {
  //   struct Node *temp;
  //   length++;
  //   temp = (struct Node *)malloc(sizeof(struct Node));
  //
  //   temp->length = message.length();
  //   temp->message = (char *)malloc(temp->length + 2);
  //
  //   strncpy(temp->message, message.c_str(), temp->length + 1);
  //   temp->message[temp->length] = '\n'; // add \n to string when enqueue
  //   temp->message[temp->length + 1] = '\0';
  //
  //   temp->next = nullptr;
  //   if (!front) {
  //     front = temp;
  //   } else {
  //     end->next = temp;
  //   }
  //   end = temp;
  // }
  //
  // char * dequeue() { // string still needs to be cleared after dequeue; be very careful about this; wherever this is called.
  //   if(length > 0) {
  //     length--;
  //     struct Node *tmp = front;
  //     char *msg = tmp->message;
  //
  //     if(length != 0) {
  //       front = tmp->next;
  //     } else {
  //       front = nullptr;
  //       end = nullptr;
  //     }
  //
  //     tmp->message = nullptr;
  //     tmp->next = nullptr;
  //     free(tmp);
  //
  //     return msg;
  //   }
        // #if DEBUG
        //   Serial.println("queue is empty. returning null pointer");
        //   Serial.flush();
        // #endif
  //   return nullptr;
  // }
//};

const int qMaxSize = 40;


struct Queue {

  struct Node {
    char message[75];
    int length; //length does include the null terminator
  };


  Node q[qMaxSize];

  uint16_t length = 0;

  int front = 0, end = 0;

  Queue() {
  }

  void enqueue(std::string message) {
    q[end].length = message.length() + 2;
    // q[end].message = (char *)malloc(q[end].length + 2);

    strncpy(q[end].message, message.c_str(), q[end].length);
    q[end].message[q[end].length] = '\n'; // add \n to string when enqueue
    q[end].message[q[end].length + 1] = '\0';

    length++;
    end++;
    if (end == qMaxSize) {
      end = 0;
    }
  }

  void dequeue(char * buffer) { // string still needs to be cleared after dequeue; be very careful about this; wherever this is called.
    if(length > 0) {
      length--;

      strncpy(buffer, q[front].message, q[front].length + 1);

      front++;
      if (front == qMaxSize) {
        front = 0;
      }
      return;
    }
    #if DEBUG
      Serial.println("queue is empty. returning null pointer");
      Serial.flush();
    #endif
  }
};

/*
 * Data structure to allow the conversion of bytes to floats and vice versa.
 */
union floatArrToBytes {
  char buffer[36];
  float sensorReadings[9];
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
