#include <Wire.h>

//on lauchpad, I2C SCL is P4.2 and SDA is P4.1
// on teensy, SCL is pin 19 and SDA is Pin 18

void setup() {
  Wire.begin();  // join i2c bus (address optional for master)
  Serial.begin(9600);
  Serial.println("finished setup");
}

void loop() {
  Wire.requestFrom(8, 5);  // request 6 bytes from slave device #8
  String response = "";
  while (Wire.available()) { // slave may send less than requested
    char c = Wire.read(); // receive a byte as character
    Serial.println(c);
    //response += c;
  }
  //double response = Wire.read();
  //Serial.println(response);
  delay(1000); //frequency of request sent out
}
