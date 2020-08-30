#include <Wire.h>

void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
}

void loop() {
  int sensor_id = 0; //change to desired sensor_id
  Wire.requestFrom(8, 4); // request 4 bytes from address 8
  uint8_t index = 0;
  union floatToBytes {
      char buffer[4];
      float voltageReading;
  } converter;
  
  while (Wire.available()){
    if (sensor_id == 0) { //successive if statements for more IDs
      converter.buffer[index] = Wire.read();
      index++;
    }
  }
  
  Serial.println(converter.voltageReading);
  delay(1000);
}
