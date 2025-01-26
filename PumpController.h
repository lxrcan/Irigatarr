#ifndef PUMP_CONTROLLER_H
#define PUMP_CONTROLLER_H

#include <Arduino.h>
#include <PubSubClient.h>
#include "Config.h"

class PumpController {
private:
    PubSubClient& client;
    bool isRunning = false;                  // Tracks if the pump is active
    unsigned long cycleStartTime = 0;        // Start of the current cycle
    unsigned long lastStopTime = 0;          // Last time the pump stopped
    unsigned long nextActionTime = 0;        // Next ON/OFF action time

    void publishStatus(const char* message) {
        client.publish(status_topic, message);
        Serial.println(message);
    }

public:
    PumpController(PubSubClient& mqttClient, int pin)
        : client(mqttClient) {
        pinMode(pump_pin, OUTPUT);
        digitalWrite(pump_pin, LOW);
    }

    void handlePumpCommand(const char* command) {
        unsigned long now = millis();

        if (String(command) == "ON") {
            // Enforce cooldown period
            if ((now - lastStopTime < cooldown_period) && cooldown_period > 0) {
                publishStatus("Pump cycle skipped due to cooldown.");
                return;
            }

            startCycle();
        } else if (String(command) == "OFF") {
            stopCycle("Stopped by command.");
        }
    }

    void manageCycle() {
        if (!isRunning) return; // If not running, skip

        unsigned long now = millis();

        // End cycle if maximum cycle time is exceeded
        if (now - cycleStartTime >= max_cycle_time) {
            stopCycle("Max cycle time reached.");
            return;
        }

        // Alternate between ON and OFF durations
        if (digitalRead(pump_pin) == HIGH && now >= nextActionTime) {
            digitalWrite(pump_pin, LOW); // Turn pump OFF
            nextActionTime = now + pump_off_duration;
            publishStatus("Pump turned OFF for rest period.");
        } else if (digitalRead(pump_pin) == LOW && now >= nextActionTime) {
            digitalWrite(pump_pin, HIGH); // Turn pump ON
            nextActionTime = now + pump_on_duration;
            publishStatus("Pump turned ON for watering.");
        }
    }

    void startCycle() {
        isRunning = true;
        cycleStartTime = millis();
        nextActionTime = millis() + pump_on_duration;
        digitalWrite(pump_pin, HIGH); // Start pump
        publishStatus("Pump cycle started.");
    }

    void stopCycle(const char* reason) {
        isRunning = false;
        lastStopTime = millis();
        digitalWrite(pump_pin, LOW); // Ensure pump is OFF
        publishStatus(reason);
        publishStatus("Pump cycle ended.");
    }
};

#endif
