#include <Wire.h>

int board_address = 0;
byte sensor_id = 0;
uint8_t index = 0;

void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);
}

union floatArrToBytes {
  char buffer[24];
  float sensorReadings[6];
} farrbconvert;

struct sensorInfo {
  String sensor_name;
  int board_address;
  byte sensor_id;
  sensorInfo(String n, int b, byte s) : sensor_name(n), board_address(b), sensor_id(s) {}
};

sensorInfo all_ids[2] = {sensorInfo("Pressure",8,0), sensorInfo("Temp",8,1)};
sensorInfo sensor = sensorInfo("",0,0);

void loop() {  
  for (int j = 0; j < sizeof(all_ids)/sizeof(all_ids[0]); j++) {
    sensor = all_ids[j];
    board_address = sensor.board_address;
    sensor_id = sensor.sensor_id;
    Serial.println(sensor.sensor_name);

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
      Serial.print(farrbconvert.sensorReadings[i]);
      Serial.print(" ");
    }
    Serial.println("");
    delay(1000);
  }
}
