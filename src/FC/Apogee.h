#pragma once

#include <Common.h>
#include "HAL.h"
#include <Comms.h>

#include <Arduino.h>

namespace Apogee {
    extern float previousAltitude;
    extern float barometerSampleRate;
    extern float barometerVelocity;
    extern uint8_t apogee;
    extern float mainParachuteAltitude;
    extern uint8_t mainChuteDeploy;

    extern float apogee; // this equals the time registered for apogee
    extern int apogeeCheck; // if 1, disallows entry into if statement
    extern int launchCheck; // becomes 1 when over 700 feet altitude
    extern int stepAvg; // defines how many values we average over
    extern float velocityAvg; // current velocity resulting from averaging past ___ (stepAvg) velocity values
    extern float apogeeTime; // stores the time at which apogee is detected/drogue deployed
    extern int mainChuteDeploy; // if 1, disallows entry into if statement
    
    void initApogee();
    float altitudeToVelocity(float altitude);
    uint32_t apogeeDetectionTask();
    // float avgVelocity(vector<float> velocityVector, int stepAvg)

};
