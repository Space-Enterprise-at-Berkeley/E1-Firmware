#include <Wire.h>
#include <SoftwareSerial.h>

SoftwareSerial RFSerial(2,3);

int board_address = 0;
byte sensor_id = 0;
uint8_t index = 0;

union floatArrToBytes {
  char buffer[24];
  float sensorReadings[6];
} farrbconvert;

struct sensorInfo {
  String sensor_name;
  int board_address;
  byte sensor_id;
  int overall_id;
  int clock_freq;
  sensorInfo(String n, int b, byte s, int o, int f) : sensor_name(n), board_address(b), sensor_id(s), overall_id(o), clock_freq(f) {}
};

sensorInfo all_ids[2] = {sensorInfo("Pressure",8,0,20,2), sensorInfo("Temp",8,1,21,1)};
sensorInfo sensor = sensorInfo("",0,0,0,0);

//Global variables for the frequency of checking sensors 
int sensor_checks[sizeof(all_ids)/sizeof(sensorInfo)][2];

void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
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
      //get a sample 
      sensor_checks[j][1] = 1;
    } else {
      sensor_checks[j][1] += 1;
      delay(100);
      continue;
    }
    sensor = all_ids[j];
    board_address = sensor.board_address;
    sensor_id = sensor.sensor_id;

    //  tell which sensor you want reading from
    Wire.beginTransmission(board_address);
    Wire.write(sensor_id);
    Wire.endTransmission();
  
    Wire.requestFrom(board_address, 24); // request 24 bytes from address 8
  
    //Get back sensor reading
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

uint16_t Fletcher16(uint8_t *data, int count) {
  
  uint16_t sum1 = 0;
  uint16_t sum2 = 0;

  for (int index=0; index<count; index++) {
    if (data[index] >0) {
      sum1 = (sum1 + data[index]) % 255;
      sum2 = (sum2 + sum1) % 255;
    }
  }
  return (sum2 << 8) | sum1;
}
