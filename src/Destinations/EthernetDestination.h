#include "Destination.h"

class EthernetDestination: public Destination {

    public:
    EthernetDestination(uint16_t port);
    void emitPacket(Comms::Packet *packet);
    bool available();
    void read(char* buffer);

    private:
    IPAddress ip_;
    IPAddress gs1_;
    IPAddress gs2_;

    EthernetUDP udp_;
    u_int16_t port_;
};