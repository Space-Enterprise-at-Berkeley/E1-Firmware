/**
 * @file IMU.cpp
 * @author Thanh T. Tran (thanhtran@berkeley.edu)
 * @brief 
 * @version 0.1
 * @date 2022-01-21
 * 
 * @copyright Copyright (c) 2022
 * @source: https://learn.adafruit.com/adafruit-bno055-absolute-orientation-sensor/arduino-code
 * @source: https://forums.adafruit.com/viewtopic.php?f=19&t=120348
 * @source: https://www.pjrc.com/teensy/pinout.html
 */

#include "IMU.h"

namespace IMU {
    uint32_t imuUpdatePeriod = 10000 * 1000;
    Comms::Packet imuPacket = {.id = 4};

    float qW = 0.0;
    float qX = 0.0;
    float qY = 0.0;
    float qZ = 0.0;
    float accelX = 0.0;
    float accelY = 0.0;
    float accelZ = 0.0;

    void initIMU() {
    }

    uint32_t imuSample(Adafruit_BNO055 *imu_ptr, uint8_t packetID, float *value) {
        /*
            getVector (adafruit_vector_type_t vector_type)
            getQuat (void)
            getTemp (void)

            VECTOR_MAGNETOMETER (values in uT, micro Teslas)
            VECTOR_GYROSCOPE (values in rps, radians per second)
            VECTOR_EULER (values in Euler angles or 'degrees', from 0..359)
            VECTOR_ACCELEROMETER (values in m/s^2)
            VECTOR_LINEARACCEL (values in m/s^2)
            VECTOR_GRAVITY (values in m/s^2)
        */ 
        imuPacket.id = packetID;
        imuPacket.len = 0;

        qW = (float) imu_ptr->getQuat().w();
        qX = (float) imu_ptr->getQuat().x();
        qY = (float) imu_ptr->getQuat().y();
        qZ = (float) imu_ptr->getQuat().z();
        accelX = (float) imu_ptr->getVector(Adafruit_BNO055::VECTOR_LINEARACCEL).x();
        accelY = (float) imu_ptr->getVector(Adafruit_BNO055::VECTOR_LINEARACCEL).y();
        accelZ = (float) imu_ptr->getVector(Adafruit_BNO055::VECTOR_LINEARACCEL).z();

        Comms::packetAddFloat(&imuPacket, qW);
        Comms::packetAddFloat(&imuPacket, qX);
        Comms::packetAddFloat(&imuPacket, qY);
        Comms::packetAddFloat(&imuPacket, qZ);
        Comms::packetAddFloat(&imuPacket, accelX);
        Comms::packetAddFloat(&imuPacket, accelY);
        Comms::packetAddFloat(&imuPacket, accelZ);
        
        Comms::emitPacket(&imuPacket);

        return imuUpdatePeriod;
    }
};
