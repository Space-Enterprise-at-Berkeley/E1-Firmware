#include <Arduino.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>

#include <string>

int port = 42069;
IPAddress ip(10, 0, 0, 42);
IPAddress remote(10, 0, 0, 69);
EthernetUDP Udp;

const uint32_t __m1 = HW_OCOTP_MAC1;
const uint32_t __m2 = HW_OCOTP_MAC0;
const byte mac[] = {
    (uint8_t)(__m1 >> 8),
    (uint8_t)(__m1 >> 0),
    (uint8_t)(__m2 >> 24),
    (uint8_t)(__m2 >> 16),
    (uint8_t)(__m2 >> 8),
    (uint8_t)(__m2 >> 0),
};

uint32_t counter = 0;

int main() {
    Serial.begin(115200);
    while(!Serial);
    Serial.println("starting");
    Serial.flush();
    Ethernet.begin((uint8_t *)mac, ip);
    Udp.begin(port);
    Serial.println("UDP begun");
    Serial.flush();

    while(1) {
        Serial.println("Starting send");
        Serial.flush();
        Udp.beginPacket(remote, port);
        Serial.println("packet begun");
        Serial.flush();
        Udp.write("hello");
        char count[10];
        Udp.write(itoa(counter, (char*) &count, 10));
        Serial.println("packet writ");
        Serial.flush();
        Udp.endPacket();
        Serial.println("packet ended");
        Serial.flush();
        counter++;
        delay(1000);
    }
    
    return 0;
}
