#include <Arduino.h>
#include <FC/BlackBox.h>
#include <Comms.h>

Comms::Packet packet = {.id = 1};

int main(){
    Serial.begin(115200);
    while (!Serial) {

    }

    Serial.println("before blackbox init");
    BlackBox::init();
    Serial.println("after blackbox init");

    #ifndef ERASE
    // write 1000 packets to blackbox
    for(uint32_t i = 0; i < 1000; i++) {
        packet.len = 0;
        Comms::packetAddUint32(&packet, i);
        Serial.printf("writing packet %i with data %i \n", i, Comms::packetGetUint32(&packet, 0));
        BlackBox::writePacket(packet, 0);
    }

    // read 1000 packets from blackbox and check if they have correct data

    //blackbox should have method to access data at certain address
    // for (uint32_t i = 0; i < 12 * 1000; i += 12) {
    //     packet = BlackBox::getData(i);
    //     uint32_t data = Comms::packetGetUint32(&packet, 0);
    //     Serial.printf("Blackbox packet %i has data %i \n", i/12, data);
    // }
    BlackBox::dumpData();
    #else
    BlackBox::erase();
    #endif
}