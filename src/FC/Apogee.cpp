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







// //////

#include "Apogee.h"
#include "Barometer.h"
#include "IMU.h"
#include <vector>

namespace Apogee {
    uint32_t apogeeUpdatePeriod = 100 * 1000;
    Comms::Packet apogeePacket = {.id = 4};
    
    float mainParachuteAltitude = 950; // feet altitude for deployment of main? 
    float datapointsCount = 0;
    float apogeeCheck = 0;
    float launchCheck = 0;
    float stepAvg = 25;
    void initApogee(){
    }
    
    uint32_t apogeeDetectionTask(uint8_t packetID,float *value) {
        apogeePacket.id = packetID;
        apogeePacket.len = 0;

        barometerVelocity = altitudeToVelocity(Barometer::altitude);
        velocityArray[datapointsCount] = barometerVelocity
        velocityAvg = 

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
    float avgVelocity(vector<float> velocityVector, int stepAvg) {
    int vectSize = velocityVector.size();
    float velocityAvg = (velocityVector[vectSize - 1] + velocityVector[vectSize - 2] + 
                        velocityVector[vectSize - 3] + velocityVector[vectSize - 4] + 
                        velocityVector[vectSize - 5] + velocityVector[vectSize - 6] + 
                        velocityVector[vectSize - 7] + velocityVector[vectSize - 8] + 
                        velocityVector[vectSize - 9] + velocityVector[vectSize - 10] + 
                        velocityVector[vectSize - 11] + velocityVector[vectSize - 12] + 
                        velocityVector[vectSize - 13] + velocityVector[vectSize - 14] + 
                        velocityVector[vectSize - 15] + velocityVector[vectSize - 16] + 
                        velocityVector[vectSize - 17] + velocityVector[vectSize - 18] + 
                        velocityVector[vectSize - 19] + velocityVector[vectSize - 20])/stepAvg;

    return velocityAvg;
}



    // check for barometer and imu flag (to check for data continuity)

    // use dh2/d2t to find acceleration for barometer
    // 

    // // compare barometer values to model via kalman filter (if not flagged)
    // // do same thing for imu

    // somehow average/weight imu and barometer acceleration values

    // find average wobble degree, 

    // if angle > threshold(~30 degrees) and barometer derived dh/dt <0 deploy parachute

