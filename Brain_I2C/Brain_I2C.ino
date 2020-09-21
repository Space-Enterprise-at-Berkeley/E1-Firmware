/*
 * Brain_I2C.ino - A c++ program that uses I2C to establish communication between 
 * the sensors and valves inside to the rocket with the ground station. Able to send
 * data to the ground station via RF. Can receive and process commands sent from 
 * ground station. 
 * Created by Vainavi Viswanath, Aug 21, 2020.
 */

#include <Wire.h>
#include <SoftwareSerial.h>

SoftwareSerial RFSerial(2,3);

int board_address = 0;
byte sensor_id = 0;
uint8_t index = 0;

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
  String sensor_name;
  int board_address;
  byte sensor_id;
  int overall_id;
  int clock_freq;
  sensorInfo(String n, int b, byte s, int o, int f) : sensor_name(n), board_address(b), sensor_id(s), overall_id(o), clock_freq(f) {}
};

/*
 * Array of all sensors we would like to get data from.
 */
sensorInfo all_ids[8] = {
  sensorInfo("Low Pressure",8,0,1,1), 
  sensorInfo("High Pressure",8,1,2,2),
  sensorInfo("Temperature",8,0,3,3),
  sensorInfo("Load Cell",8,1,4,4),
  sensorInfo("GPS",8,0,5,5),
  sensorInfo("Barometer",8,1,6,6),
  sensorInfo("Board Telemetry",8,0,7,7),
  sensorInfo("Radio Telemetry",8,1,8,8)
};

sensorInfo sensor = sensorInfo("",0,0,0,0);

/* 
 *  Stores how often we should be requesting data from each sensor.
 */
int sensor_checks[sizeof(all_ids)/sizeof(sensorInfo)][2];

void setup() {
  Wire.begin();       
  Serial.begin(9600);
  RFSerial.begin(57600);
  for (int i=0; i<sizeof(all_ids)/sizeof(sensorInfo); i++) {
    sensor_checks[i][0] = all_ids[i].clock_freq;
    sensor_checks[i][1] = 1;
  }
}

void loop() {  
  for (int j = 0; j < sizeof(all_ids)/sizeof(sensorInfo); j++) {
    if (sensor_checks[j][0] == sensor_checks[j][1]) {
      sensor_checks[j][1] = 1;
    } else {
      sensor_checks[j][1] += 1;
      delay(100);
      continue;
    }
    sensor = all_ids[j];
    board_address = sensor.board_address;
    sensor_id = sensor.sensor_id;

    Wire.beginTransmission(board_address);
    Wire.write(sensor_id);
    Wire.endTransmission();
    Wire.requestFrom(board_address, 24); 
  
    index = 0;
    while (Wire.available()){
      farrbconvert.buffer[index] = Wire.read();
      index++;
    }
    for (int i=0; i<6; i++) {
      float reading = farrbconvert.sensorReadings[i];
      if (reading > 0) {
      }
    }
    String packet = make_packet(sensor);
    Serial.println(packet);
    RFSerial.println(packet);
    delay(100);
  }
}

/*
 * Constructs packet in the following format: 
 * {<sensor_ID>, <data1>, <data2>, ..., <dataN>|checksum}
 */
String make_packet(struct sensorInfo sensor) {
  String packet_content = (String)sensor.overall_id;
  packet_content += ",";
  for (int i=0; i<6; i++) {
    float reading = farrbconvert.sensorReadings[i];
    if (reading > 0) {
      packet_content += (String)reading;
      packet_content += ",";
    } else {
      packet_content.remove(packet_content.length()-1);
      int count = packet_content.length();
      char const *data = packet_content.c_str();
      uint16_t checksum = Fletcher16((uint8_t *) data, count);
      packet_content += "|";
      packet_content += (String)checksum;
      break;
    }
  }
  String packet = "{" + packet_content + "}";
  return packet;
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
