#include "Actuators.h"

namespace Actuators {

    // TODO: change this to appropriate value
    uint32_t actuatorCheckPeriod = 50 * 1000;

    Actuator pressFlowRBV = { .actuatorID = 0,
                    .statusPacketID = 13,
                    .voltage = 0.0,
                    .current = 0.0,
                    .period = 100 * 1000,
                    .state = 0,
                    .pin1 = HAL::hBridge1Pin1,
                    .pin2 = HAL::hBridge1Pin2,
                    .stop = NULL, 
                    .muxChannel = &HAL::muxChan13};
  
    void driveForwards(Actuator *actuator){
        digitalWriteFast(actuator->pin1, HIGH);
        digitalWriteFast(actuator->pin2, LOW);
        actuator->state = 1;
    }

    void driveBackwards(Actuator *actuator){
        digitalWriteFast(actuator->pin1, LOW);
        digitalWriteFast(actuator->pin2, HIGH);
        actuator->state = 2;
    }

    void stopAct(Actuator *actuator){
        digitalWriteFast(actuator->pin1, LOW);
        digitalWriteFast(actuator->pin2, LOW);
        actuator->state = 0;
    }

    void brakeAct(Actuator *actuator){
        digitalWriteFast(actuator->pin1, HIGH);
        digitalWriteFast(actuator->pin2, HIGH);
        actuator->state = 4; // Probably won't brake
    }

    void extendPressFlowRBV(){ driveForwards(&pressFlowRBV); }
    void retractPressFlowRBV(){ driveBackwards(&pressFlowRBV); }
    uint32_t stopPressFlowRBV(){ stopAct(&pressFlowRBV); pressFlowRBV.stop->enabled = false; return 0;}
    void brakePressFlowRBV(){ brakeAct(&pressFlowRBV); }
    void pressFlowRBVPacketHandler(Comms::Packet tmp, uint8_t ip){ actPacketHandler(tmp, &extendPressFlowRBV, &retractPressFlowRBV,
    
     pressFlowRBV.stop); }

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

    //common function for sampling H bridges with the MUX
    void sampleActuator(Actuator *actuator) {
        actuator->current = actuator->muxChannel->readChannel1();
        actuator->voltage = actuator->muxChannel->readChannel2();
        DEBUG("Actuator current: ");
        DEBUG(actuator->current);
        DEBUG("\n");
        DEBUG_FLUSH();

        if (actuator->current > OClimit){
            switch(actuator->actuatorID){
                case 0: stopPressFlowRBV(); break;
                // case 1: stopAct1(); break;
                // case 2: stopAct2(); break;
            }
            actuator->state = 3;
        }
 
        if (actuator->state > 0 && !actuator->stop->enabled && actuator->current < stopCurrent){
            switch(actuator->actuatorID){
                case 0: stopPressFlowRBV(); break;
                // case 1: stopAct1(); break;
                // case 2: stopAct2(); break;
            }
        }

        Comms::Packet tmp = {.id = actuator->statusPacketID};
        Comms::packetAddUint8(&tmp, actuator->state);
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

    void initActuators(Task *pressFlowStopTask) {
        Comms::registerCallback(169, pressFlowRBVPacketHandler);
        pressFlowRBV.stop = pressFlowStopTask;
    }
};