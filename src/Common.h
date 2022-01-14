#pragma once

#include <Arduino.h>

#ifdef DEBUG_MODE
#define DEBUG(val) Serial.print(val)
#else
#define DEBUG(val)
#endif

#ifdef DEBUG_MODE
#define DEBUG_FLUSH() Serial.flush()
#else
#define DEBUG_FLUSH()
#endif

struct Task {
    uint32_t (*taskCall)(void);
    uint32_t nexttime;
    bool enabled = true;
};
