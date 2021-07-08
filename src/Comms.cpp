#include <Comms.h>

namespace Comms {
  EthernetUDP udp;
  char packet[75];
  void (*handlers[256])(float *values, uint8_t count);


  void handleArmValve(float *values, uint8_t count) {
    if(values[0] == 1.0) {
      Valves::arm.open();
    } else if(values[0] == 0.0) {
      Valves::arm.close();
    }

  }

  void handleLoxMainValve(float *values, uint8_t count) {
    Valves::arm.open();
  }

  void handlePropMainValve(float *values, uint8_t count) {
    Valves::arm.open();
  }

  void init() {
    handlers.insert({1, &handleArmValve});
    handlers.insert({2, &handleLoxMainValve});
    handlers.insert({3, &handlePropMainValve});
  }

  void listen()
  {
    int pktSize = udp.parsePacket();
    if(pktSize) {
      udp.read(packet, pktSize);
      // process command
      // run corresponding function in comms class
    }
  }

  uint16_t fletcher16(uint8_t *data, int count)
  {
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
}
