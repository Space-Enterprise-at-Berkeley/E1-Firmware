#include <tests/TestHeaters.h>
#include <Heaters.h>
#include <Scheduler.h>

namespace TestHeater
{
    testFunc::testFunc() {
        /*  construct a testFunc instance
            here we initialize all the stuff we need, in order to test:
            - Heater class
            - &Wire
            - Serial monitor
        */
        Serial.begin(57600);
        htrClass = Heaters::initHeater(&Wire1); // Heaters use Wire1
        heatStart = millis();
    }

    void testFunc::loop() {
        /*  Run basic functionality test:
            - continuously read voltage
            - continuously read currents
            - turn heater on and off
            - transfer data through Serial.println
        */
        if (((millis() - heatStart) > 2000) && ((millis() - heatStart) < 12000)) {
            // turn on ALL the heaters
            for (int i = 0; i < htrClass->numHeaters; i++) {
                // put current temperature to 0 to force heaters on
                htrClass->controlTemp(0, i, 0);
            }
        } else if ((millis() - heatStart) > 12000) {
            // turn off ALL heaters
            for (int i = 0; i < htrClass->numHeaters; i++) {
                // put current temperature to 200 (setpoint is 10) to force heaters off
                htrClass->controlTemp(0, i, 200);
            }
        }
        if ((millis() - lastPrint) > 200) { // don't want to block scheduler
            Serial.println("\nReading Current Draw:");
            for (int i = 0; i < htrClass->numHeaters; i++) {
                htrClass->readCurrentDraw(0, i);
                Serial.print(htrClass->currents[i]); Serial.print("\t");
            }
            Serial.print("\n");
            Serial.println("Reading Bus Voltages:");
            for (int i = 0; i < htrClass->numHeaters; i++) {
                htrClass->readBusVoltage(0, i);
                Serial.print(htrClass->voltages[i]); Serial.print("\t");
            }
            Serial.print("\n");
            Serial.println("Checking Overcurrents:");
            for (int i = 0; i < htrClass->numHeaters; i++) {
                htrClass->checkOverCurrent(0, i);
                Serial.print(htrClass->overCurrents[i]); Serial.print("\t");
            }
            Serial.println("\n");
            lastPrint = millis();
        }
    }
    
}
