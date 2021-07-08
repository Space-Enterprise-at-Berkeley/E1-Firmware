#pragma once

#include <Valves.h>

#include <Arduino.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>

namespace Comms {
  extern EthernetUDP udp;
  extern char packet[75];
  extern void (*handlers[256])(float *values, uint8_t count);

  // command RPCs
  void handleArmValve(float *values, uint8_t count);
  void handleLoxMainValve(float *values, uint8_t count);
  void handlePropMainValve(float *values, uint8_t count);

  void init();
  void listen(); // listen for incoming packets
  uint16_t fletcher16(uint8_t *data, int count);
}
