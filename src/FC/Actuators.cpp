#include "Actuators.h"

namespace Actuators {

    // TODO: change this to appropriate value
    uint32_t actuatorCheckPeriod = 50 * 1000;

    // TODO: set correct telem packet IDs
    // Comms::Packet pressFlowRBVPacket = {.id = 39};
    // uint8_t pressFlowRBVState = 0;
    // float pressFlowRBVVoltage = 0.0;
    // float pressFlowRBVCurrent = 0.0;
    // Task *stoppressFlowRBV;

    Actuator pressFlowRBV = {.statusPacketID = 39,
                    .hasVoltage = true,
                    .hasCurrent = true,
                    .voltage = 0.0,
                    .current = 0.0,
                    .period = 100 * 1000,
                    .state = 0,
                    .pin1 = HAL::hBridge2Pin1,
                    .pin2 = HAL::hBridge2Pin2,
                    .muxChannel = &HAL::muxChan14};
  
    void driveForwards(uint8_t pin1, uint8_t pin2, uint8_t *actState, uint8_t actuatorID){
        digitalWriteFast(pin1, HIGH);
        digitalWriteFast(pin2, LOW);
        actuator->state = 1;
    }

    void driveBackwards(uint8_t pin1, uint8_t pin2, uint8_t *actState, uint8_t actuatorID){
        digitalWriteFast(pin1, LOW);
        digitalWriteFast(pin2, HIGH);
        actuator->state = 2;
    }

    void stopAct(uint8_t pin1, uint8_t pin2, uint8_t *actState, uint8_t actuatorID){
        digitalWriteFast(pin1, LOW);
        digitalWriteFast(pin2, LOW);
        actuator->state = 0;
    }

    void brakeAct(uint8_t pin1, uint8_t pin2, uint8_t *actState, uint8_t actuatorID){
        digitalWriteFast(pin1, HIGH);
        digitalWriteFast(pin2, HIGH);
        actuator->state = 4; // Probably won't brake
    }

    void extendpressFlowRBV(){ driveForwards(pressFlowRBVPin1, pressFlowRBVPin2, &pressFlowRBVState, 0); }
    void retractpressFlowRBV(){ driveBackwards(pressFlowRBVPin1, pressFlowRBVPin2, &pressFlowRBVState, 0); }
    uint32_t stoppressFlowRBV(){ stopAct(pressFlowRBVPin1, pressFlowRBVPin2, &pressFlowRBVState, 0); stopPressFlowRBV->enabled = false; return 0;}
    void brakepressFlowRBV(){ brakeAct(pressFlowRBVPin1, pressFlowRBVPin2, &pressFlowRBVState, 0); }
    void pressFlowRBVPacketHandler(Comms::Packet tmp, uint8_t ip){ actPacketHandler(tmp, &extendpressFlowRBV, &retractpressFlowRBV, stopPressFlowRBV); }

    void actPacketHandler(Comms::Packet tmp, void (*extend)(), void (*retract)(), Task *stopTask){
/*         switch(tmp.data[0]){
            case 0:
                (*extend)();
                break;
            case 1:
                (*retract)();
                break;
            case 2:
                (*extend)();
                if(stopTask->enabled) stopTask->nexttime += Comms::packetGetUint32(&tmp, 1) * 1000;
                else stopTask->nexttime = micros() + Comms::packetGetUint32(&tmp, 1) * 1000;
                stopTask->enabled = true;
                break;
            case 3:
                (*retract)();
                if(stopTask->enabled) stopTask->nexttime += Comms::packetGetUint32(&tmp, 1) * 1000;
                else stopTask->nexttime = micros() + Comms::packetGetUint32(&tmp, 1) * 1000;
                stopTask->enabled = true;
                break;
        } */

        // (Actuate code) 0: extend fully 1: retract fully 2: extend millis 3: retract millis

        if(tmp.data[0]%2)(*extend)();
        else (*retract)();

        if(tmp.data[0]>1){
            uint32_t actuatetime = Comms::packetGetUint32(&tmp, 1);
            if(stopTask->enabled) stopTask->nexttime += actuatetime * 1000;
            else stopTask->nexttime = micros() + actuatetime * 1000;
            stopTask->enabled = true;
        }
    }

    // void sampleActuator(Comms::Packet *packet, INA219 *ina, float *voltage, float *current, uint8_t *actState, uint8_t actuatorID) {
    //     *voltage = ina->readBusVoltage();
    //     *current = ina->readShuntCurrent();

    //     if (*current > OClimits[actuatorID]){
    //         switch(actuatorID){
    //             case 0: stoppressFlowRBV(); break;
    //             // case 1: stopAct1(); break;
    //             // case 2: stopAct2(); break;
    //         }
    //         *actState = 3;
    //     }

    //     if ((*actState == 1 || *actState == 2) && *current < stopCurrent){
    //         switch(actuatorID){
    //             case 0: stoppressFlowRBV(); break;
    //             // case 1: stopAct1(); break;
    //             // case 2: stopAct2(); break;
    //         }
    //     }

    //     packet->len = 0;
    //     Comms::packetAddUint8(packet, *actState);
    //     Comms::packetAddFloat(packet, *voltage);
    //     Comms::packetAddFloat(packet, *current);
    //     Comms::emitPacket(packet);
    // }

    //common function for sampling H bridges with the MUX
    void sampleActuator(Actuator *actuator) {
        if (actuator->hasVoltage) {
            actuator->voltage = actuator->muxChannel->readChannel1();
        }
        if (actuator->hasCurrent) {
            actuator->current = actuator->muxChannel->readChannel2();
        }

        if (actuator->current > OClimits[actuatorID]){
            switch(actuatorID){
                case 0: stoppressFlowRBV(); break;
                // case 1: stopAct1(); break;
                // case 2: stopAct2(); break;
            }
            actuator->state = 3;
        }

        if ((actuator->state == 1 || actuator->state == 2) && actuator->current < stopCurrent){
            switch(actuatorID){
                case 0: stoppressFlowRBV(); break;
                // case 1: stopAct1(); break;
                // case 2: stopAct2(); break;
            }
        }

        Comms::Packet tmp = {.id = actuator->statusPacketID};
        Comms::packetAddUint8(&tmp, actuator->actState);
        Comms::packetAddFloat(&tmp, actuator->voltage);
        Comms::packetAddFloat(&tmp, actuator->current);
        Comms::emitPacket(&tmp);
    }



    uint32_t pressFlowRBVSample() {
        sampleActuator(&pressFlowRBV);
        return actuatorCheckPeriod;
    }

    // uint32_t act1Sample() {
    //     sampleActuator(&act1);
    //     return actuatorCheckPeriod;
    // }

    // uint32_t act2Sample() {
    //     sampleActuator(&act2);
    //     return actuatorCheckPeriod;
    // }

    void initActuators() {
        Comms::registerCallback(169, pressFlowRBVPacketHandler);
    }
};