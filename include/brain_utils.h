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

#include <SPI.h>
#include <string>
#include <SdFat.h>
#include <TimeLib.h>

String make_packet(struct sensorInfo sensor);
uint16_t Fletcher16(uint8_t *data, int count);
void chooseValveById(int id, struct valveInfo *valve);

const int numCryoTherms = 2;
int cryoThermAddrs[numCryoTherms] = {0x60, 0x67};
_themotype cryoTypes[numCryoTherms] = {MCP9600_TYPE_J, MCP9600_TYPE_T};

const int numADCSensors = 2;
int ADSAddrs[numADCSensors] = {0b1001010, 0b1001000};
int adcDataReadyPins[numADCSensors] = {29, 28};
ADS1219 ** ads;

const int numAnalogThermocouples = 1;
int thermAdcIndices[numAnalogThermocouples] = {1};
int thermAdcChannels[numAnalogThermocouples] = {2};

const int numPressureTransducers = 5;
int ptAdcIndices[numPressureTransducers] = {0, 0, 0, 0, 1};
int ptAdcChannels[numPressureTransducers] = {0, 1, 2, 3, 0};

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

//---------Startup & Shutdown-------------

bool startup = false;
int startupPhase = 0;

/* Delays during startup sequence:
  1 - Between open pressure and open LOX Main
  2 - Between open LOX Main and open Prop Main
  3 - Between open Prop Main and close Main Valve Arm
*/
int startupDelays[3] = {1000, 0 ,1000};
uint32_t startupTimer;

bool shutdown = false;

int beginBFlow();
void startupConfirmation(float *data);

bool checkStartupProgress(int startupPhase, int startupTimer);
bool advanceStartup(int startupPhase);

int endBFlow();


//---------SD Queue-------------

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

//----------------------

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
  {"Perform Flow", 29, &(beginFlow), &(endFlow), &(startupConfirmation)},
  {"Perform BFLow", 30, &(beginBFlow), &(endBFlow), &(startupConfirmation)}
};

void sensorReadFunc(int id) {
  switch (id) {
    case 0:
      Thermocouple::Analog::readTemperatureData(farrbconvert.sensorReadings);
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
 * {<valve_ID>,<open(1) or close(0)|checksum}
 * Populated the fields of the valve and returns the action to be taken
 */
int decode_received_packet(String packet, valveInfo *valve) {
  int data_start_index = packet.indexOf(',');
  if(data_start_index == -1) {
    return -1;
  }
  int valve_id = packet.substring(1,data_start_index).toInt();
  const int data_end_index = packet.indexOf('|');
  if(data_end_index == -1) {
    return -1;
  }
  int action = packet.substring(data_start_index + 1,data_end_index).toInt();

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
    chooseValveById(valve_id, valve);
    return action;
  } else {
    return -1;
  }
}

void chooseValveById(int id, valveInfo *valve) {
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


// Pretending is a valve action, do -1 to indicate that
int beginFlow() {
  /* Check if rocket is in required state for a flow:
    Pressure - Closed
    LOX GEMS & Prop GEMS - Open
    Arming Valve - Closed
    LOX Main Valve & Prop Main Valve - Closed
  */
  //&& Solenoids::getLoxGems() && Solenoids::getPropGems()
  startup = !Solenoids::getHPS() &&
      !Solenoids::getLox2() && !Solenoids::getLox5() && !Solenoids::getProp5();
  return -1;
}

int beginBFlow() {
  /* Check if rocket is in required state for a flow:
    Pressure - Closed
    LOX GEMS & Prop GEMS - Closed
    Arming Valve - Closed
    LOX Main Valve & Prop Main Valve - Closed
  */
  startup = !Solenoids::getHPS() &&
      !Solenoids::getLox2() && !Solenoids::getLox5() && !Solenoids::getProp5() &&
      !Solenoids::getLoxGems() && !Solenoids::getPropGems();
      return -1;
}

void startupConfirmation(float *data) {
  data[0] = startup ? 1 : 0;
  data[1] = -1;
}

// Pretending is a valve action, do -1 to indicate that
int endFlow() {
  shutdown = true;
  return 1;
}

int endBFlow() {
  /* Simultaneously:
      2 Way - Open
      Pressurant - Closed
      Propane - Closed
      Lox - Closed
      GEMS - Open
  */
  Solenoids::armAll();
  Solenoids::deactivateHighPressureSolenoid();
  Solenoids::closePropane();
  Solenoids::closeLOX();
  Solenoids::ventLOXGems();
  Solenoids::ventPropaneGems();
  Solenoids::getAllStates(farrbconvert.sensorReadings);
  shutdown = true;
  return 1;
}

bool checkStartupProgress(int startupPhase, int startupTimer) {
  if (startupPhase > 0) {
    if (startupTimer > startupDelays[startupPhase - 1]) {
      return true;
    } else {
      return false;
    }
  } else {
    return true;
  }
}

/*
  Progress startup given the current startup phase
*/
bool advanceStartup() {
  if (startupPhase == 0) {

        //close both GEMS, open Arming valve, open Pressurant
        Solenoids::closeLOXGems();
        Solenoids::closePropaneGems();
        Solenoids::armLOX();
        Solenoids::activateHighPressureSolenoid();

        Solenoids::getAllStates(farrbconvert.sensorReadings);
        startupPhase++;

  } else if (startupPhase == 1) {

      // after a delay open LOX main valve
      Solenoids::openLOX();
      Solenoids::getAllStates(farrbconvert.sensorReadings);
      startupPhase++;

  } else if (startupPhase == 2) {

      // after a delay open Prop main valve
      Solenoids::openPropane();
      Solenoids::getAllStates(farrbconvert.sensorReadings);
      startupPhase++;

  } else if (startupPhase == 3) {

      // after a delay close Arming valve
      Solenoids::disarmLOX();
      Solenoids::getAllStates(farrbconvert.sensorReadings);


      startupPhase = 0;
      startup = false;

  }
  else {
    return false;
  }
  return true;
}
