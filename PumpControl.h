#ifndef PUMP_CONTROL_H
#define PUMP_CONTROL_H

#include <Arduino.h>
#include <PubSubClient.h>

class PumpControl {
private:
    PubSubClient& client;
    int pumpPin;
    const char* statusTopic;
    const char* controlTopic;

    bool manualMode = false; // Tracks if manual mode is active
    bool isPumpRunning = false; // Tracks if the pump is currently running

    unsigned long lastActivationTime = 0; // Tracks the last time the pump was activated
    unsigned long lastStopTime = 0; // Tracks the last time the pump stopped
    unsigned long cooldownPeriod = 600000; // Default cooldown time (10 minutes)
    
    unsigned long onDuration = 300000; // Pump run time per cycle (5 minutes)
    unsigned long offDuration = 600000; // Rest time between cycles (10 minutes)
    unsigned long pumpCycleStartTime = 0; // Tracks the start of the pump cycle

    int lowerThreshold = 0; // Soil moisture level below which the pump starts
    int upperThreshold = 1000; // Soil moisture level above which the pump stops

    void publishStatus(const char* message) {
        client.publish(statusTopic, message);
        Serial.println(message);
    }

public:
    PumpControl(PubSubClient& mqttClient, int pumpPin, const char* statusTopic, const char* controlTopic)
        : client(mqttClient), pumpPin(pumpPin), statusTopic(statusTopic), controlTopic(controlTopic) {}

    void setManualMode(bool mode) {
        manualMode = mode;
        publishStatus(manualMode ? "Manual mode enabled." : "Automatic mode enabled.");
    }

    void setThresholds(int lower, int upper) {
        lowerThreshold = lower;
        upperThreshold = upper;
    }

    void setCooldownPeriod(unsigned long cooldown) {
        cooldownPeriod = cooldown;
    }

    void setCycleDurations(unsigned long onTime, unsigned long offTime) {
        onDuration = onTime;
        offDuration = offTime;
    }

    void checkPumpCycle(int soilValue) {
        unsigned long now = millis();

        // Enforce cooldown period
        if ((now - lastStopTime) < cooldownPeriod && cooldownPeriod > 0) {
            return; // Skip if still in cooldown
        }

        if (manualMode) {
            // Manual mode: Trigger via Home Assistant
            if (getHomeAssistantManualTrigger() && !isPumpRunning) {
                publishStatus("Starting pump cycle (manual mode).");
                startPumpCycle();
            }
        } else {
            // Automatic mode: Trigger based on soil moisture
            if (soilValue < lowerThreshold && !isPumpRunning) {
                publishStatus("Starting pump cycle (automatic mode, moisture below threshold).");
                startPumpCycle();
            }
        }

        // Handle pump running logic
        if (isPumpRunning) {
            if ((now - pumpCycleStartTime) >= onDuration) {
                publishStatus("Pump cycle ended (time duration reached).");
                stopPump();
            }
        } else {
            if ((now - lastStopTime) >= offDuration) {
                // Check if soil moisture exceeds upper threshold
                if (soilValue > upperThreshold) {
                    publishStatus("Pump cycle not restarted (moisture above upper threshold).");
                    return; // End irrigation session
                } else {
                    publishStatus("Restarting pump cycle (off duration completed).");
                    startPumpCycle(); // Start next cycle
                }
            }
        }
    }

    void startPumpCycle() {
        digitalWrite(pumpPin, HIGH);
        isPumpRunning = true;
        pumpCycleStartTime = milli   s();
        publishStatus("Pump started.");
    }

    void stopPump() {
        digitalWrite(pumpPin, LOW);
        isPumpRunning = false;
        lastStopTime = millis();
        publishStatus("Pump stopped.");
    }

    bool getHomeAssistantManualTrigger() {
        // Placeholder: Replace with actual MQTT or button trigger logic
        return false;
    }
};

#endif
