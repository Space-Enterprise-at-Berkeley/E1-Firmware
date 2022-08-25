#include "MuxChannel.h"

void MuxChannel::init(Mux *mux, int channelID, float ch1ScalingFactor = 1, float ch2ScalingFactor = 1) {
        MuxChannel::mux = mux;
        MuxChannel::channelID = channelID;
        MuxChannel::ch1ScalingFactor = ch1ScalingFactor;
        MuxChannel::ch2ScalingFactor = ch2ScalingFactor;
}

float MuxChannel::readChannel1() {
    digitalWrite(mux->muxSelect1Pin, channelID & 1);
    digitalWrite(mux->muxSelect2Pin, channelID >> 1 & 1);
    digitalWrite(mux->muxSelect3Pin, channelID >> 2 & 1);
    digitalWrite(mux->muxSelect4Pin, channelID >> 3 & 1);
    delayMicroseconds(1);
    // (float)analogRead(aPin0) * 3.3 / 4096.0;
    return ch1ScalingFactor * analogRead(mux->muxInput1Pin);
}

float MuxChannel::readChannel2() {
    digitalWrite(mux->muxSelect1Pin, channelID & 1);
    digitalWrite(mux->muxSelect2Pin, channelID >> 1 & 1);
    digitalWrite(mux->muxSelect3Pin, channelID >> 2 & 1);
    digitalWrite(mux->muxSelect4Pin, channelID >> 3 & 1);
    delayMicroseconds(1);
    return ch2ScalingFactor * analogRead(mux->muxInput2Pin);
}