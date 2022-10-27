#pragma once

#include <stdint.h>
#include "PCA9539.h"
#include <Arduino.h>
#include "Comms.h"

namespace OCHandler {
    extern PCA9539 ioOCExpander;

    extern uint32_t ocUpdatePeriod;

    extern bool ocDetected;
    
    void initOCHandler(uint8_t interruptPin);
    void OCInterrupt();
    uint32_t handleOC();
}
