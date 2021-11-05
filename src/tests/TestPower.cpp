#include <tests/TestPower.h>
#include <Scheduler.h>
#include <Power.h>
#include <string>

namespace TestPower
{
    testReadouts::testReadouts() {
        /*  construct a testReadouts instance
            here we initialize all the stuff we need, in order to test:
            - Power class
            - &Wire
            - Serial monitor
        */
        Serial.begin(57600);
        powerClass = Power::initPower(&Wire); // powerSupplyMonitor is on Wire
    }

    void testReadouts::loop() {
        /*  Run basic functionality test:
            - read voltage
            - read currents
            - read power consumption
            - transfer data through Serial.println
        */
        Serial.println("\nReading Currents:");
        for (int i = 0; i < powerClass->numSupplies; i++) {
            powerClass->readCurrent(0, i);
            Serial.print(powerClass->currents[i]); Serial.print("\t");
        }
        Serial.print("\n");
        Serial.println("Reading Voltages:");
        for (int i = 0; i < powerClass->numSupplies; i++) {
            powerClass->readVoltage(0, i);
            Serial.print(powerClass->voltages[i]); Serial.print("\t");
        }
        Serial.print("\n");
        Serial.println("Reading Power Consumption:");
        for (int i = 0; i < powerClass->numSupplies; i++) {
            powerClass->readPowerConsumption(0, i);
            Serial.print(powerClass->powers[i]); Serial.print("\t");
        }
        Serial.println("\n");
    }

    testPowerTasks::testPowerTasks() {
        /*  construct a testReadouts instance
            here we initialize all the stuff we need, in order to test:
            - Power class
            - &Wire
            - Serial monitor
            - Scheduler
        */
        Serial.begin(57600);
        powerClass = Power::initPower(&Wire); // powerSupplyMonitor is on Wire

        // Create and schedule tasks
        Serial.println("\nTesting Task creation, scheduling, and execution for Power.");
        for (uint8_t i = 0; i < powerClass->numSupplies; i++) {
            Power::PowerTask * voltageTask = new Power::PowerTask(powerClass, i, &Power::Power::readVoltage);
            Power::PowerTask * currTask = new Power::PowerTask(powerClass, i, &Power::Power::readCurrent);
            Power::PowerTask * powerConsTask = new Power::PowerTask(powerClass, i, &Power::Power::readPowerConsumption);
            // Schedule voltage readings to take place at 0.5s intervals
            Scheduler::repeatTask(voltageTask, 500000);
            // Schedule current readings to take place at 0.1s intervals
            Scheduler::repeatTask(currTask, 100000);
            // Schedule power consumption readings to take place at 1.5s intervals
            Scheduler::repeatTask(powerConsTask, 1500000);
        }
        Serial.println("PowerTasks scheduled...");
    }

    void testPowerTasks::loop() {
        /*  Test Task creation, scheduling, and execution 
            Outputs have to be manually inspected using Serial monitor :P
        */
        if ((millis() - lastPrint) > 200) { // don't want to block scheduler
            Serial.println("\nReading Currents:");
            for (int i = 0; i < powerClass->numSupplies; i++) {
                Serial.print(powerClass->currents[i]); Serial.print("\t");
            }
            Serial.print("\n");
            Serial.println("Reading Voltages:");
            for (int i = 0; i < powerClass->numSupplies; i++) {
                Serial.print(powerClass->voltages[i]); Serial.print("\t");
            }
            Serial.print("\n");
            Serial.println("Reading Power Consumption:");
            for (int i = 0; i < powerClass->numSupplies; i++) {
                Serial.print(powerClass->powers[i]); Serial.print("\t");
            }
            Serial.println("\n");
            lastPrint = millis();
        }
    }

} 

