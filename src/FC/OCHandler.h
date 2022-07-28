#pragma once

#include <stdint.h>
#include "PCA9539.h"
#include <Arduino.h>
#include "Comms.h"

namespace OCHandler {
    extern PCA9539 ioOCExpander;
    
    void initOCHandler(uint8_t interruptPin);
    void OCInterrupt();
}
