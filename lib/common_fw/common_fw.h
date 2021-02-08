/*
 * common_fw.h- A c++ program that houses the firmware that is reasonably hardware
 * agnostic and can be repeated across different boards and flight hardware.
 * Created by Vainavi Viswanath, Aug 21, 2020.
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <SPI.h>
#include <iostream>
#include <string>
#include <SdFat.h>
#include <TimeLib.h>
#include <Arduino.h>
#include <Wire.h>

#define PACKET_START 0x2A56
#define PACKET_END 0xC51F

struct Queue {

  struct Node {
    struct Node *next;
    uint8_t *message;
    uint8_t length; //length doesn't include the null terminator
  };

  uint16_t length = 0;

  struct Node *end = 0;
  struct Node *front = 0;

  Queue() {
    length = 0;
    end = nullptr;
    front = nullptr;
  }

  void enqueue(uint8_t *message, uint8_t size) {
    struct Node *temp;
    length++;
    temp = (struct Node *)malloc(sizeof(struct Node));

    temp->length = size;
    temp->message = (uint8_t *)malloc(temp->length + 2);

    strncpy(temp->message+1, message, temp->length + 1);
    temp->message[temp->length + 1] = '\n'; // add \n to string when enqueue

    temp->message[0] = temp->length; // first element of this char array has its length.

    temp->next = nullptr;
    if (!front) {
      front = temp;
    } else {
      end->next = temp;
    }
    end = temp;
  }

  /**
   * string still needs to be cleared after dequeue;
   * be very careful about this; wherever this is called.
   */
  char * dequeue() {
    if(length > 0) {
      length--;
      struct Node *tmp = front;
      char *msg = tmp->message;

      if(length != 0) {
        front = tmp->next;
      } else {
        front = nullptr;
        end = nullptr;
      }

      tmp->message = nullptr;
      tmp->next = nullptr;
      free(tmp);

      return msg;
    }
    return nullptr;
  }
};

/*
 * Data structure to allow the conversion of bytes to floats and vice versa.
 */
union floatArrToBytes {
  char buffer[28];
  float sensorReadings[7];
};

/*
 * Data structure to store all information relevant to a specific sensor type.
 */
struct sensorInfo {
  std::string name;
  int board_address;
  int id;
  int clock_freq;
};

/*
 * Data structure to store all information relevant to a specific valve.
 */
struct valveInfo {
  String name;
  uint8_t id;
  uint8_t num_data;
  int (*openValve)();
  int (*closeValve)();
  void (*ackFunc)(float *data);
};


uint8_t make_packet (uint8_t id, bool error);
uint16_t Fletcher16 (uint8_t *data, int count);
void chooseValveById (uint8_t id, struct valveInfo *valve, valveInfo valves[], int numValves);
bool write_to_SD(uint8_t *message, uint8_t size, const char * file_name);
bool write_to_SD(std::string message, const char * file_name);
int8_t decode_received_packet(uint8_t *_command, valveInfo *valve, valveInfo valves[], int numValves);
uint8_t read_and_process_input(valveInfo *valve, valveInfo valves[], int numValves, HardwareSerial *ser);
void take_action(valveInfo *valve, int action);
uint16_t Fletcher16(uint8_t *data, int count);
void debug(std::string str);
void debug(uint8_t *bytes, int num);

extern SdFat sd;
extern File file;

extern char packet[64];
extern char command[64]; //input command from GS

extern struct Queue *sdBuffer;

extern union floatArrToBytes farrbconvert;
extern struct sensorInfo *sensors;
extern struct valveInfo *valves;

#endif // _COMMON_H_
