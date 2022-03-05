#include "Apogee.h"
#include "Barometer.h"
#include "IMU.h"

namespace Apogee {
    uint32_t apogeeUpdatePeriod = 100 * 1000;
    Comms::Packet apogeePacket = {.id = 4};
    
    float mainParachuteAltitude = 950; // feet altitude for deployment of main? 
    
    // float accelerationThreshold = -2;  // change this and check units?
    float velocityThreshold = -2; // change this and check units?
    
    void initApogee(){
    }
    
    uint32_t apogeeDetectionTask(uint8_t packetID,float *value) {
        apogeePacket.id = packetID;
        apogeePacket.len = 0;

        // barometerAcceleration = altitudeToAcceleration(Barometer::altitude);
        barometerVelocity = altitudeToVelocity(Barometer::altitude);
        
        if (barometerVelocity < velocityThreshold) {
            // deploy drogue
            apogee = 1;
        }

        if (apogee == 1 && Barometer::altitude < mainParachuteAltitude){
            // deploy main chute
            mainChuteDeploy = 1;
        }


        Comms::packetAddUint8(&apogeePacket, apogee);
        Comms::packetAddUint8(&apogeePacket, mainChuteDeploy);
        Comms::emitPacket(&apogeePacket);

        return apogeeUpdatePeriod;
    }

    float altitudeToAcceleration(float altitude) {
        float velocity = (altitude - previousAltitude) / barometerSampleRate;
        float barometerAcceleration = (velocity - previousVelocity) / barometerSampleRate;

        previousAltitude = altitude;
        previousVelocity = velocity;

        return barometerAcceleration;
    
    float altitudeToVelocity(float altitude) {
        float velocity = (altitude - previousAltitude) / barometerSampleRate;

        previousAltitude = altitude;
        previousVelocity = velocity;

        return barometerVelocity;
    }
}



    // check for barometer and imu flag (to check for data continuity)

    // use dh2/d2t to find acceleration for barometer
    // 

    // // compare barometer values to model via kalman filter (if not flagged)
    // // do same thing for imu

    // somehow average/weight imu and barometer acceleration values

    // find average wobble degree, 

    // if angle > threshold(~30 degrees) and barometer derived dh/dt <0 deploy parachute
