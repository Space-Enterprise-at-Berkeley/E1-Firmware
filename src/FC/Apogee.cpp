#include "Apogee.h"

namespace Apogee {
    Task *_disableCommsTask;
    bool altitudeValid = false;
    bool descending = false;

    void init() {
    }

    void startAltitudeDetection() {
        altitudeValid = true;
    }

    uint32_t disableCommsTask() {
        if (altitudeValid) {
            if (!descending && Barometer::baroAltitude > 100) {
                descending = true;
            } else if (descending && Barometer::baroAltitude < 50) {
                Comms::toggleTelemetry(false);
            }
        }

        return 5000;
    }
}