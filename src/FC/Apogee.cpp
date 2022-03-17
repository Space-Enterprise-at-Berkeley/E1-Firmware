#include "Apogee.h"
#include "Barometer.h"
#include "IMU.h"


namespace Apogee {
    uint32_t apogeeUpdatePeriod = 100 * 1000;
    Comms::Packet apogeePacket = {.id = 158};
    
    float mainParachuteAltitude = 950; // feet altitude for deployment of main? 

    uint32_t apogeeTime = 0;

    uint8_t apogeeCheck = 0;
    uint8_t launchCheck = 0;

    const int stepAvg = 25;
    float velocitySamples[stepAvg] = {0};
    int velocityIndex = 0;

    uint32_t mainChuteDeploy = 0;
    uint32_t mainChuteDeployTime;

    float previousAltitude = 0;
    float previousVelocity = 0;
    float barometerVelocity = 0;
    float velocityAvg = 0;

    void initApogee(){
    }

    void startApogeeDetection() {
        launchCheck = 1;
    }
    
    uint32_t apogeeDetectionTask() {
        barometerVelocity = altitudeToVelocity(Barometer::baroAltitude);
        velocitySamples[velocityIndex] = barometerVelocity;
        velocityIndex = (velocityIndex + 1) % stepAvg;
        
        float runningSum = 0;
        for (int i = 0; i < stepAvg; i++) {
            runningSum = runningSum + velocitySamples[i];
        }
        velocityAvg = runningSum / stepAvg;

        if ((velocityAvg < 0) && (apogeeCheck == 0) && (launchCheck == 1))  {
            apogeeCheck = 1;
            apogeeTime = millis();
        }

        if ((Barometer::baroAltitude < mainParachuteAltitude) && (mainChuteDeploy == 0) && apogeeCheck){
            // deploy main chute
            mainChuteDeploy = 1;
            mainChuteDeployTime = millis();
        }

        apogeePacket.len = 0;
        Comms::packetAddUint32(&apogeePacket, apogeeTime);
        Comms::packetAddUint32(&apogeePacket, mainChuteDeployTime);
        Comms::emitPacket(&apogeePacket);

        return apogeeUpdatePeriod;
    }

    
    float altitudeToVelocity(float altitude) {
        float velocity = (altitude - previousAltitude) / Barometer::bmUpdatePeriod;

        previousAltitude = altitude;
        previousVelocity = velocity;

        return barometerVelocity;
    }
}

