/**
 * @file main.cpp
 * @author Thanh T. Tran (thanhtran@berkeley.edu)
 * @brief 
 * @version 0.1
 * @date 2022-01-23
 * 
 * @copyright Copyright (c) 2022
 * @source: https://learn.adafruit.com/adafruit-bno055-absolute-orientation-sensor/arduino-code
 * @source: https://forums.adafruit.com/viewtopic.php?f=19&t=120348
 * @source: https://www.pjrc.com/teensy/pinout.html
 */
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <BNO055.h>
#include <utility/imumaths.h>

#include <string>

BNO055 bno = BNO055(28);
/*
    Useful functions:
    getVector (adafruit_vector_type_t vector_type)
    getQuat (void)
    getTemp (void)
    
    adafruit_vector_type_t data types:
    VECTOR_MAGNETOMETER (values in uT, micro Teslas)
    VECTOR_GYROSCOPE (values in rps, radians per second)
    VECTOR_EULER (values in Euler angles or 'degrees', from 0..359)
    VECTOR_ACCELEROMETER (values in m/s^2)
    VECTOR_LINEARACCEL (values in m/s^2)
    VECTOR_GRAVITY (values in m/s^2)
*/

int main() {
    Serial.begin(115200);
    while (!Serial) { 

    }
    Serial.println("Orientation Sensor Test"); Serial.println("");
    Wire.begin();
    Wire.setClock(100000);
    
    /* Initialize the sensor */
    if(!bno.begin())
    {
        /* There was a problem detecting the BNO055 ... check your connections */
        Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
        while(1);
    }
    
    delay(1000);
        
    while(1) {
        Serial.println("Starting send");
        Serial.flush();

        if (bno.m_highGDetected) {
            Serial.println("high_G flag read!");
            bno.m_highGDetected = false;//if there was an interrupt, reset the flag
            //If we got here becuase of a HW interup we need to clear the interupt flag
            bno.write8(BNO055::BNO055_SYS_TRIGGER_ADDR, B01000000);  
        }
        //flash an LED on for 2 seconds to show interrupt was read
        // pinMode(13, OUTPUT); 
        // digitalWrite(13, HIGH);
        // delay(2000);
        // digitalWrite(13, LOW); 
        // delay(500);

        sensors_event_t event; 
        bno.getEvent(&event);

        /* Display the floating point data */
        Serial.print("X: ");
        Serial.print(event.orientation.x);
        Serial.print("\tY: ");
        Serial.print(event.orientation.y);
        Serial.print("\tZ: ");
        Serial.print(event.orientation.z);
        Serial.println("");
        Serial.flush();

        
        /* Display the acceleration data */
        Serial.print("AccelX: ");
        Serial.print(bno.getVector(BNO055::VECTOR_LINEARACCEL).x());
        Serial.print("\tAccelY: ");
        Serial.print(bno.getVector(BNO055::VECTOR_LINEARACCEL).y());
        Serial.print("\tAccelZ: ");
        Serial.print(bno.getVector(BNO055::VECTOR_LINEARACCEL).z());
        Serial.println("");
        Serial.flush();

        /* Display the quat data */
        imu::Quaternion quat = bno.getQuat();
        Serial.print("qW: ");
        Serial.print(quat.w(), 4);
        Serial.print(" qX: ");
        Serial.print(quat.y(), 4);
        Serial.print(" qY: ");
        Serial.print(quat.x(), 4);
        Serial.print(" qZ: ");
        Serial.print(quat.z(), 4);
        Serial.println("");
        Serial.flush();


        /* Display the current temperature */
        int8_t temp = bno.getTemp();
        Serial.print("Current Temperature: ");
        Serial.print(temp);
        Serial.println(" C");
        Serial.println("");
        Serial.flush();

        delay(100);
    }
    
    return 0;
}

/************************************************************************************/


// #include <Common.h>
// #include <Comms.h>
// #include "HAL.h"
// #include "IMU.h"

// #include <Arduino.h>
// #include <Wire.h>
// #include <SPI.h>

// Task taskTable[] = {
//     // imu
//     {IMU::imuSample, 0},
// };

// #define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

// int main() {
//     // hardware setup
//     Serial.begin(115200);
//     #ifdef DEBUG_MODE
//     while(!Serial) {} // wait for user to open serial port (debugging only)
//     #endif
//     HAL::initHAL();
//     Comms::initComms();
//     IMU::initIMU();

//     while(1) {
//         for(uint32_t i = 0; i < TASK_COUNT; i++) { // for each task, execute if next time >= current time
//             uint32_t ticks = micros(); // current time in microseconds
//             if (taskTable[i].nexttime - ticks > UINT32_MAX / 2 && taskTable[i].enabled) {
//                 taskTable[i].nexttime = ticks + taskTable[i].taskCall();
//             }
//         }
//         Comms::processWaitingPackets();
//     }
//     return 0;
// }
