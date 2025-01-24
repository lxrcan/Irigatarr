#ifndef PUMP_CONTROL_H
#define PUMP_CONTROL_H

#include <Arduino.h>
#include <PubSubClient.h>

class PumpControl {
private:
    PubSubClient& mqttClient;
    int pumpPin;
    const char* statusTopic;
    const char* controlTopic;
    unsigned long lastActivationTime;
    bool isRunning;
    const unsigned long maxRunTime = 20000; // Max pump run time in milliseconds

public:
    PumpControl(PubSubClient& client, int pin, const char* status, const char* control)
        : mqttClient(client), pumpPin(pin), statusTopic(status), controlTopic(control), lastActivationTime(0), isRunning(false) {}

    void init() {
        pinMode(pumpPin, OUTPUT);
        digitalWrite(pumpPin, LOW);
    }

    void handleMessage(const String& topic, const String& message) {
        if (topic == String(controlTopic)) {
            if (message == "ON") {
                activatePump();
            } else if (message == "OFF") {
                deactivatePump();
            } else {
                Serial.println("Invalid pump control command received.");
            }
        }
    }

    void checkTimeout() {
        if (isRunning && millis() - lastActivationTime > maxRunTime) {
            Serial.println("Pump stopped due to timeout.");
            deactivatePump();
        }
    }

private:
    void activatePump() {
        Serial.println("Activating pump.");
        digitalWrite(pumpPin, HIGH);
        isRunning = true;
        lastActivationTime = millis();
        mqttClient.publish(statusTopic, "Pump turned ON");
    }

    void deactivatePump() {
        Serial.println("Deactivating pump.");
        digitalWrite(pumpPin, LOW);
        isRunning = false;
        mqttClient.publish(statusTopic, "Pump turned OFF");
    }
};

#endif
