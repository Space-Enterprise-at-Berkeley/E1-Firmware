#include "SerialDestination.h"

SerialDestination::SerialDestination(HardwareSerial *serial, long baud) {
    serial_ = serial;

    serial_->begin(baud);
}

void SerialDestination::emitPacket(Comms::Packet *packet) {
    serial_->write(packet->id);
    serial_->write(packet->len);
    serial_->write(packet->timestamp, 4);
    serial_->write(packet->checksum, 2);
    serial_->write(packet->data, packet->len);
    serial_->write('\n');
}