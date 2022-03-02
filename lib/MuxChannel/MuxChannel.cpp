#include "MuxChannel.h"

void MuxChannel::init(Mux *mux, int channelID) {
        MuxChannel::mux = mux;
        MuxChannel::channelID = channelID;
}

void MuxChannel::setScalingFactor(float scalingFactor) {
    MuxChannel::scalingFactor = scalingFactor;
}

float MuxChannel::readChannel1() {
    digitalWrite(mux->muxSelect1Pin, channelID & 1);
    digitalWrite(mux->muxSelect2Pin, channelID >> 1 & 1);
    digitalWrite(mux->muxSelect3Pin, channelID >> 2 & 1);
    digitalWrite(mux->muxSelect4Pin, channelID >> 3 & 1);
    return scalingFactor * analogRead(mux->muxInput1Pin);
}

float MuxChannel::readChannel2() {
    digitalWrite(mux->muxSelect1Pin, channelID & 1);
    digitalWrite(mux->muxSelect2Pin, channelID >> 1 & 1);
    digitalWrite(mux->muxSelect3Pin, channelID >> 2 & 1);
    digitalWrite(mux->muxSelect4Pin, channelID >> 3 & 1);
    return scalingFactor * analogRead(mux->muxInput2Pin);
}