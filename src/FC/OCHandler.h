#pragma once

#include <stdint.h>
#include "PCA9539.h"
#include <Arduino.h>
#include "Comms.h"

namespace OCHandler {
    PCA9539 ioOCExpander(0x74);

    void initOCHandler();
}
