#include <SPIFlash.h>

#define SS_FLASHMEM   25

uint16_t expectedDeviceID=0xEF40;
SPIFlash flash(SS_FLASHMEM, expectedDeviceID);

void initFlash() {
  pinMode(SS, OUTPUT); digitalWrite(SS, HIGH);

  if (flash.initialize())
  {
    Serial.println("Init OK! Starting flash dump");
  }
  else {
    Serial.print("Init FAIL, expectedDeviceID(0x");
    Serial.print(expectedDeviceID, HEX);
    Serial.print(") mismatched the read value: 0x");
    Serial.println(flash.readDeviceId(), HEX);
  }
   
}

void setup() {
  delay(5000);
  Serial.begin(115200); //set up serial over usb
  Serial.println("---FLASH DUMP---");
  initFlash();
  for (int i = 0; i < 20000000; i++) {
    Serial.print(flash.readByte(i), 16);
    Serial.flush();
  }
}

void loop() {

}