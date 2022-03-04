#include "Camera.h"

namespace Camera {
    void init() {
        Comms::registerCallback(156, &startCamera);
    }

    void startCamera(Comms::Packet packet) {
        Serial7.write(startArray, 5);
        Serial7.flush();
    }
}