#include <SPIFlash.h>    //get it here: https://github.com/LowPowerLab/SPIFlash
#define SERIAL_BAUD      115200
char input = 0;
long lastPeriod = -1;
#define LED_BUILTIN 2
#define SS_FLASHMEM 22
//////////////////////////////////////////
// flash(SPI_CS, MANUFACTURER_ID)
// SPI_CS          - CS pin attached to SPI flash chip (8 in case of Moteino)
// MANUFACTURER_ID - OPTIONAL, 0x1F44 for adesto(ex atmel) 4mbit flash
//                             0xEF30 for windbond 4mbit flash
//                             0xEF40 for windbond 64mbit flash
//////////////////////////////////////////
uint16_t expectedDeviceID=0xEF40;
SPIFlash flash(SS_FLASHMEM, expectedDeviceID);


void initFlash() {
  pinMode(SS, OUTPUT); digitalWrite(SS, HIGH);

  if (flash.initialize())
  {
    Serial.println("Init OK!");
  }
  else {
    Serial.print("Init FAIL, expectedDeviceID(0x");
    Serial.print(expectedDeviceID, HEX);
    Serial.print(") mismatched the read value: 0x");
    Serial.println(flash.readDeviceId(), HEX);
  }
}
void setup(){
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  initFlash();
  delay(4000);
  Serial.println("starting erase");
  flash.chipErase();
  while (flash.busy());
  Serial.println("erased");


}

void loop(){
}