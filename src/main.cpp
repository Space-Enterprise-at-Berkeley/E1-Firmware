#include "Comms.h"
#include "Common.h"

using namespace Comms;

Packet p;

void setup() {
  delay(5000);
  Serial.begin(115200); //set up serial over usb
  Serial2.begin(115200); // set up radio
}

void loop() {
  p.id = 1;
  p.len = 0;
  packetAddUint8(&p, 0xAA);

  emitPacket(&p);

  delay(1000);
}