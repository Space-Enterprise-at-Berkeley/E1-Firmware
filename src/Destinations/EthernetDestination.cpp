#include "EthernetDestination.h"

EthernetDestination::EthernetDestination() {
    port_ = 42069;

    udp_.begin(port_);
}

EthernetDestination::emitPacket(Comms::Packet *packet) {
    return NULL;
}

