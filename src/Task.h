#pragma once

#include <Arduino.h>

struct Task {
    uint32_t (*taskCall)(void);
    uint32_t nexttime;
};
