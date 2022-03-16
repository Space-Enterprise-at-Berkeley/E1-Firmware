#include "Automation.h"

namespace Automation {

    void init() {
        Comms::registerCallback(157, &configureLaunch);
    }

    void configureLaunch(Comms::Packet packet) {
        Apogee::startAltitudeDetection();
        BlackBox::beginWrite(packet);
    }

}