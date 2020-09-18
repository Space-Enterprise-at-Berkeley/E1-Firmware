#include <Wire.h>
#include <SoftwareSerial.h>

SoftwareSerial RFSerial(2,3);

int board_address = 0;
byte sensor_id = 0;
uint8_t index = 0;

void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);
  RFSerial.begin(57600);
}

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

sensorInfo all_ids[2] = {sensorInfo("Pressure",8,0,20,1000), sensorInfo("Temp",8,1,21,500)};
sensorInfo sensor = sensorInfo("",0,0,0,0);

void loop() {  
  for (int j = 0; j < sizeof(all_ids)/sizeof(all_ids[0]); j++) {
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
    delay(sensor.clock_freq);
  }
}

String make_packet(struct sensorInfo sensor) {
  String packet = "{";
  packet += (String)sensor.overall_id;
  packet += ",";
  char *data = 0;
  int count = 0;
  for (int i=0; i<6; i++) {
    float reading = farrbconvert.sensorReadings[i];
    if (reading > 0) {
      packet += (String)reading;
      data += (char)reading;
      packet += ",";
      count += 1;
    } else {
      packet.remove(packet.length()-1);
      packet += "|";
      break;
    }
  }
  packet += (String)Fletcher16(data, count);
  packet += "}";
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
