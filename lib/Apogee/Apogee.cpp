#include "Apogee.h"
#include "Comms.h"
//baro altitudes all in meters
#define APOGEE_THRESHOLD_ALTITUDE 700 //FAR altitude (600) + 100meters
#define MAIN_PARACHUTE_ALTITUDE 1500 //FAR altitude(600) + 3k ft (900m)
#define UPDATE_PERIOD 100000 //100ms, 10hz

namespace Apogee {
    uint32_t apogeeUpdatePeriod = UPDATE_PERIOD;
    Comms::Packet apogeePacket = {.id = 158};
    
    float mainParachuteAltitude = MAIN_PARACHUTE_ALTITUDE;

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
    
    void apogeeDetectionTask(Comms::Packet* aP, float baroAltitude) {
        barometerVelocity = altitudeToVelocity(baroAltitude);
        barometerVelocity = 1;
        velocitySamples[velocityIndex] = barometerVelocity;
        velocityIndex = (velocityIndex + 1) % stepAvg;
        
        float runningSum = 0;
        for (int i = 0; i < stepAvg; i++) {
            runningSum = runningSum + velocitySamples[i];
        }
        velocityAvg = runningSum / stepAvg;

        if ((velocityAvg < 0) && (apogeeCheck == 0) && (launchCheck == 1) && baroAltitude > APOGEE_THRESHOLD_ALTITUDE)  {
        
            apogeeCheck = 1;
            apogeeTime = millis();
        }

        if ((baroAltitude < mainParachuteAltitude) && (mainChuteDeploy == 0) && apogeeCheck){
            // deploy main chute
            mainChuteDeploy = 1;
            mainChuteDeployTime = millis();
        }

        aP->len = 0;
        Comms::packetAddUint32(aP, apogeeTime);
        Comms::packetAddUint32(aP, mainChuteDeployTime);

        

    }

    
    float altitudeToVelocity(float altitude) {

        float velocity = (altitude - previousAltitude) / UPDATE_PERIOD;

        previousAltitude = altitude;
        previousVelocity = velocity;

        return barometerVelocity;
    }
}
