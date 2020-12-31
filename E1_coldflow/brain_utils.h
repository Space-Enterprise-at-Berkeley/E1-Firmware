/*
 * Brain_utils.h- A c++ program that uses I2C to establish communication between
 * the sensors and valves inside to the rocket with the ground station. Able to send
 * data to the ground station via RF. Can receive and process commands sent from
 * ground station. This contains additional functions and structs used in Brain_I2C.ino.
 * Created by Vainavi Viswanath, Aug 21, 2020.
 */
#include <solenoids.h>
#include <recovery.h>
#include <ducer.h>
#include <Thermocouple.h>
#include <tempController.h>
#include <batteryMonitor.h>
//#include <commands.h>

#include <SPI.h>
#include <string>
#include <SdFat.h>
#include <TimeLib.h>

#define FLIGHT_BRAIN_ADDR 0x00

std::string str_file_name = "E1_speed_test_results.txt";
const char * file_name = str_file_name.c_str();

String make_packet(struct sensorInfo sensor);
uint16_t Fletcher16(uint8_t *data, int count);
void chooseById(int id, struct valveInfo *valve);
void readTemp();

const int numCryoTherms = 2;
int cryoThermAddrs[numCryoTherms] = {0x60, 0x67}; //the second one is 6A or 6B, not sure which for Addr pin set to 1/2
_themotype cryoTypes[numCryoTherms] = {MCP9600_TYPE_J, MCP9600_TYPE_T};

// SD_FAT_TYPE = 0 for SdFat/File as defined in SdFatConfig.h,
// 1 for FAT16/FAT32, 2 for exFAT, 3 for FAT16/FAT32 and exFAT.
#define SD_FAT_TYPE 0

#if SD_FAT_TYPE == 0
SdFat sd;
File file;
#elif SD_FAT_TYPE == 1
SdFat32 sd;
File32 file;
#elif SD_FAT_TYPE == 2
SdExFat sd;
ExFile file;
#elif SD_FAT_TYPE == 3
SdFs sd;
FsFile file;
#endif  // SD_FAT_TYPE

struct Queue {

  struct Node {
    struct Node *next;
    char *message;
    int length; //length doesn't include the null terminator
  };

  uint16_t length = 0;

  struct Node *end = 0;
  struct Node *front = 0;

  Queue() {
    length = 0;
    end = nullptr;
    front = nullptr;
  }

  void enqueue(std::string message) {
    struct Node *temp;
    length++;
    temp = (struct Node *)malloc(sizeof(struct Node));

    temp->length = message.length();
    temp->message = (char *)malloc(temp->length + 2);

    strncpy(temp->message, message.c_str(), temp->length + 1);
    temp->message[temp->length] = '\n'; // add \n to string when enqueue
    temp->message[temp->length + 1] = '\0';

    temp->next = nullptr;
    if (!front) {
      front = temp;
    } else {
      end->next = temp;
    }
    end = temp;
  }

  char * dequeue() { // string still needs to be cleared after dequeue; be very careful about this; wherever this is called.
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

struct Queue *sdBuffer;

/*
 * Data structure to allow the conversion of bytes to floats and vice versa.
 */
union floatArrToBytes {
  char buffer[28];
  float sensorReadings[7];
} farrbconvert;

/*
 * Data structure to store all information relevant to a specific sensor type.
 */
struct sensorInfo {
  String name;
  int board_address;
  int id;
  int clock_freq;
  void (*readData)(float *data);
};

/*
 * Data structure to store all information relevant to a specific valve.
 */
struct valveInfo {
  String name;
  int id;
  int (*openValve)();
  int (*closeValve)();
  void (*ackFunc)(float *data);
};

const uint8_t numSensors = 8;

/*
   Array of all sensors we would like to get data from.
*/
sensorInfo sensors[numSensors] = {
  // local sensors
  {"Temperature",                FLIGHT_BRAIN_ADDR, 0, 3, &(readTemp)}, //&(testTempRead)}, //&(Thermocouple::readTemperatureData)},
  {"All Pressure",               FLIGHT_BRAIN_ADDR, 1, 1, &(Ducers::readAllPressures)},
  {"Battery Stats",              FLIGHT_BRAIN_ADDR, 2, 3, &(batteryMonitor::readAllBatteryStats)},
//  {"Load Cells",                 FLIGHT_BRAIN_ADDR, 3, 5},
  {"Aux temp",                   FLIGHT_BRAIN_ADDR, 4, 1, &(Thermocouple::Cryo::readCryoTemps)},

//  {"Solenoid Ack",               FLIGHT_BRAIN_ADDR, 4, -1},
//  {"Recovery Ack",               FLIGHT_BRAIN_ADDR, 5, -1},

  //  {"GPS",                        -1, -1, 7, 5, NULL}, //&(GPS::readPositionData)},
  //  {"GPS Aux",                    -1, -1, 8, 8, NULL}, //&(GPS::readAuxilliaryData)},
  //  {"Barometer",                  -1, -1, 8, 6, NULL}, //&(Barometer::readAltitudeData)},
  //  {"Load Cell Engine Left",      -1, -1, 9,  5, NULL},
  //  {"Load Cell Engine Right",     -1, -1, 10, 5, NULL}
};

const int numValves = 9;

valveInfo valves[numValves] = {
  {"LOX 2 Way", 20, &(Solenoids::armLOX), &(Solenoids::disarmLOX), &(Solenoids::getAllStates)},
  {"LOX 5 Way", 21, &(Solenoids::openLOX), &(Solenoids::closeLOX), &(Solenoids::getAllStates)},
  {"LOX GEMS", 22, &(Solenoids::ventLOXGems), &(Solenoids::closeLOXGems), &(Solenoids::getAllStates)},
  {"Propane 2 Way", 23, &(Solenoids::armPropane), &(Solenoids::disarmPropane), &(Solenoids::getAllStates)},
  {"Propane 5 Way", 24, &(Solenoids::openPropane), &(Solenoids::closePropane), &(Solenoids::getAllStates)},
  {"Propane GEMS", 25, &(Solenoids::ventPropaneGems), &(Solenoids::closePropaneGems), &(Solenoids::getAllStates)},
  {"High Pressure Solenoid", 26, &(Solenoids::activateHighPressureSolenoid), &(Solenoids::deactivateHighPressureSolenoid), &(Solenoids::getAllStates)},
  {"Arm Rocket", 27, &(Solenoids::armAll), &(Solenoids::disarmAll), &(Solenoids::getAllStates)},
  {"Launch Rocket", 28, &(Solenoids::LAUNCH), &(Solenoids::endBurn), &(Solenoids::getAllStates)},
};



//void sensorReadFunc(int id) {
//  switch (id) {
//    case 0:
//      readTemp();
//      break;
//    case 1:
//      Ducers::readAllPressures(farrbconvert.sensorReadings);
//      break;
//    case 2:
//      batteryMonitor::readAllBatteryStats(farrbconvert.sensorReadings);
//      break;
//    case 4:
//      Thermocouple::Cryo::readCryoTemps(farrbconvert.sensorReadings);
//      break;
//    default:
//      Serial.println("some other sensor");
//      break;
//  }
//}

void readTemp() {
  Ducers::readTemperatureData(farrbconvert.sensorReadings);
  farrbconvert.sensorReadings[1] = tempController::controlTemp(farrbconvert.sensorReadings[0]);
  farrbconvert.sensorReadings[2] = -1;
}

/*
 * Calls the corresponding method for this valve with the appropriate
 * action in solenoids.h
 */
void take_action(valveInfo *valve, sensorInfo *sensor, int action) {
  if (action == 1) {
    valve->openValve();
  } else if (action == 0) {
    valve->closeValve();
  } else if (action == 2) {
    //call the reading function
    sensor->readData(farrbconvert.sensorReadings);
  }
  if(action != -1 && action != 2)
    valve->ackFunc(farrbconvert.sensorReadings);
}

/*
 * Constructs packet in the following format:
 * {<sensor_ID>,<data1>,<data2>, ...,<dataN>|checksum}
 */
String make_packet(int id, bool error) {
  String packet_content = (String)id;
  packet_content += ",";
  if (!error) {
    for (int i=0; i<7; i++) {
      float reading = farrbconvert.sensorReadings[i];
      if (reading != -1) {
        packet_content += (String)reading;
        packet_content += ",";
      } else {
        break;
      }
    }
  } else {
    packet_content += "0,";
  }
  
  packet_content.remove(packet_content.length()-1);
  int count = packet_content.length();
  char const *data = packet_content.c_str();
  uint16_t checksum = Fletcher16((uint8_t *) data, count);
  packet_content += "|";
  String check_ = String(checksum, HEX);
  while(check_.length() < 4) {
    check_ = "0" + check_;
  }
  packet_content += check_;
  String packet = "{" + packet_content + "}";

  return packet;
}

/*
 * Decodes a packet sent from ground station in the following format:
 * {<ID>,<command (optional)>|checksum}
 * Populated the fields of the valve and returns the action to be taken
 */
int decode_received_packet(String packet, valveInfo *valve, sensorInfo *sensor) {
  Serial.println(packet);
  int data_start_index = packet.indexOf(',');
  boolean data_request = false;
  int action = 2;
  if(data_start_index == -1) {
    data_request = true;
  }
  int id = packet.substring(1,data_start_index).toInt();
  const int data_end_index = packet.indexOf('|');
  if(data_end_index == -1) {
    return -1;
  }
  if (!data_request) {
    int action = packet.substring(data_start_index + 1,data_end_index).toInt();
  }

  String checksumstr = packet.substring(data_end_index + 1, packet.length()-1);
  char *checksum_char = checksumstr.c_str();
  int checksum = strtol(checksum_char, NULL, 16);
  Serial.println(checksum);

  const int count = packet.substring(1, data_end_index).length(); // sanity check; is this right? off by 1 error?
  String str_data= packet.substring(1,data_end_index);
  char const *data = str_data.c_str(); 
  Serial.println(data);

  int _check = (int)Fletcher16((uint8_t *) data, count);
  Serial.println(_check);
  if (_check == checksum) {
    Serial.println("Checksum correct, taking action");
    return action;
  } else {
    return -1;
  }
}

void chooseById(int id, valveInfo *valve, sensorInfo *sensor) {
  for (int i = 0; i < numValves; i++) {
    if (valves[i].id == id) {
      *valve = valves[i];
      return;
    }
  }
  for (int i=0; i < numSensors; i++) {
    if (sensors[i].id == id) {
      *sensor = sensors[i];
      return;
    }
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

bool write_to_SD(std::string message) {
  // every reading that we get from sensors should be written to sd and saved.

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
      } else {                                                            //If the file didn't open
        return false;
      }
    }
    return true;
}
