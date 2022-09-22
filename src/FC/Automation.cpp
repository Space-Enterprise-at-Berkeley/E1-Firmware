#include "Automation.h"

namespace Automation {

    Comms::Packet flightModePacket = {.id = 55};
    uint8_t flightMode = 0;

    void init() {
        Comms::registerCallback(157, &configureLaunch);
    }

    void configureLaunch(Comms::Packet packet) {
        Apogee::startApogeeDetection();
        BlackBox::beginWrite(packet);
        flightMode = 1;
    }

    uint32_t sendFlightModePacket() {
        flightModePacket.len = 0;
        Comms::packetAddUint8(&flightModePacket, flightMode);
        Comms::emitPacket(&flightModePacket);

        DEBUG("FLIGHT MODE: ");
        DEBUG(flightMode);
        DEBUG("\n");

        return 200 * 1000;
    }

}