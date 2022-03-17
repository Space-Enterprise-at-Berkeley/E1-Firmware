#include "Apogee.h"
#include "Barometer.h"
#include "IMU.h"


namespace Apogee {
    uint32_t apogeeUpdatePeriod = 100 * 1000;
    Comms::Packet apogeePacket = {.id = 4};
    
    float mainParachuteAltitude = 950; // feet altitude for deployment of main? 
    uint32_t apogeeTime = 0;
    int apogee = 0;
    float apogeeCheck = 0;
    float launchCheck = 0;
    const float stepAvg = 25;
    uint32_t mainChuteDeploy = 0;
    uint32_t mainChuteDeployTime;
    float velocitySamples[stepAvg] = {0};
    int velocityIndex = 0;
    float previousAltitude = 0;
    float previousVelocity = 0;
    float barometerVelocity = 0;
    float velocityAvg = 0;
    

    
    

    void initApogee(){
    }
    
    uint32_t apogeeDetectionTask(uint8_t packetID,float *value) {
        apogeePacket.id = packetID;
        apogeePacket.len = 0;

        barometerVelocity = altitudeToVelocity(Barometer::altitude);
        velocitySamples[velocityIndex] = barometerVelocity;
        
        float runningSum = 0;

        for (int i = 0; i < stepAvg; i++) {
            runningSum = runningSum + velocitySamples[i];
        }

        velocityAvg = runningSum / stepAvg;
        
        if (Barometer::altitude > 700) {
            launchCheck = 1;
        }

        if ((velocityAvg < 0 ) && (apogeeCheck == 0) && (launchCheck == 1))  {
            apogee = 1;
            apogeeCheck = 1;
            apogeeTime = millis()


            ///send info to deploy drogue
        }

        if ((apogee == 1) && (Barometer::altitude < mainParachuteAltitude) && (mainChuteDeploy == 0)){
            // deploy main chute
            mainChuteDeploy = 1;
            mainChuteDeployTime = millis();
        }
        velocityIndex = (velocityIndex + 1) % stepAvg


        Comms::packetAddUint32(&apogeePacket, apogeeTime);
        Comms::packetAddUint32(&apogeePacket, mainChuteDeployTime);
        Comms::emitPacket(&apogeePacket);

        return apogeeUpdatePeriod;
    }

    
    float altitudeToVelocity(float altitude) {
        float velocity = (altitude - previousAltitude) / barometerSampleRate;

        previousAltitude = altitude;
        previousVelocity = velocity;

        return barometerVelocity;
    }




    
    // float avgVelocity(vector<float> velocityVector, int stepAvg) {
    // int vectSize = velocityVector.size();
    // float velocityAvg = (velocityVector[vectSize - 1] + velocityVector[vectSize - 2] + 
    //                     velocityVector[vectSize - 3] + velocityVector[vectSize - 4] + 
    //                     velocityVector[vectSize - 5] + velocityVector[vectSize - 6] + 
    //                     velocityVector[vectSize - 7] + velocityVector[vectSize - 8] + 
    //                     velocityVector[vectSize - 9] + velocityVector[vectSize - 10] + 
    //                     velocityVector[vectSize - 11] + velocityVector[vectSize - 12] + 
    //                     velocityVector[vectSize - 13] + velocityVector[vectSize - 14] + 
    //                     velocityVector[vectSize - 15] + velocityVector[vectSize - 16] + 
    //                     velocityVector[vectSize - 17] + velocityVector[vectSize - 18] + 
    //                     velocityVector[vectSize - 19] + velocityVector[vectSize - 20])/stepAvg;

    // return velocityAvg;
}

