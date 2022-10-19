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
    uint32_t imuUpdatePeriod = 30 * 1000;
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

    uint32_t imuSample() {
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

        auto q = HAL::bno055.getQuat();
        auto a = HAL::bno055.getVector(BNO055::VECTOR_LINEARACCEL);
        qW = (float) q.w();
        qX = (float) q.x();
        qY = (float) q.y();
        qZ = (float) q.z();
        accelX = (float) a.x();
        accelY = (float) a.y();
        accelZ = (float) a.z();

        imuPacket.len = 0;

        DEBUG("Accelerometer X: ");
        DEBUG(accelX);
        DEBUG("     Y: ");
        DEBUG(accelY);
        DEBUG("     Z:");
        DEBUG(accelZ);
        DEBUG("\n");
        DEBUG_FLUSH();

        Comms::packetAddFloat(&imuPacket, qW);
        Comms::packetAddFloat(&imuPacket, qX);
        Comms::packetAddFloat(&imuPacket, qY);
        Comms::packetAddFloat(&imuPacket, qZ);
        Comms::packetAddFloat(&imuPacket, accelX);
        Comms::packetAddFloat(&imuPacket, accelY);
        Comms::packetAddFloat(&imuPacket, accelZ);
                
        Comms::emitPacket(&imuPacket);
        Comms::emitPacket(&imuPacket, &RADIO_SERIAL, "\r\n\n", 3);

        return imuUpdatePeriod;
    }
};