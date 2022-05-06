#pragma once



#include <Comms.h>
#include <Common.h>

#include <Arduino.h>

namespace Apogee {    
    void initApogee();

    void startApogeeDetection();
    float altitudeToVelocity(float altitude);
    void apogeeDetectionTask(Packet* apogeePacket);

};