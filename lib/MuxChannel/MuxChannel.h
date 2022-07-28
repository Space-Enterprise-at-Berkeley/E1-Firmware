#pragma once

#include <stdint.h>
#include <Arduino.h>

struct Mux {
    uint8_t muxSelect1Pin;
    uint8_t muxSelect2Pin;
    uint8_t muxSelect3Pin;
    uint8_t muxSelect4Pin;
    uint8_t muxInput1Pin;
    uint8_t muxInput2Pin;
};

class MuxChannel {
    public:
    float scalingFactor = 1;
    void init(Mux *mux, int channelID);
    void setScalingFactor(float scalingFactor);
    float readChannel1();
    float readChannel2();
    private:
    Mux *mux;
    int channelID;
};