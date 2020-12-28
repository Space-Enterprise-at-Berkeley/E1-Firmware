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
#include <commands.h>

#include <SPI.h>
#include <string>
#include <SdFat.h>
#include <TimeLib.h>

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

const int numValves = 10;

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

void sensorReadFunc(int id) {
  switch (id) {
    case 0:
      //Thermocouple::setSensor(0);
      Ducers::readTemperatureData(farrbconvert.sensorReadings);
      farrbconvert.sensorReadings[1] = tempController::controlTemp(farrbconvert.sensorReadings[0]);
      farrbconvert.sensorReadings[2] = -1;
      break;
    case 1:
      Ducers::readAllPressures(farrbconvert.sensorReadings);
      break;
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
