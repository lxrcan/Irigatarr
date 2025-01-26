#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include "DHT.h"
#include <PubSubClient.h>
#include "Config.h"

class SensorManager {
private:
    PubSubClient& client;
    DHT& dhtSensor;

    void publishSensorData(const char* topic, float value) {
        if (!isnan(value)) {
            client.publish(topic, String(value).c_str());
            Serial.print(topic);
            Serial.print(": ");
            Serial.println(value);
        } else {
            Serial.print("Failed to read ");
            Serial.println(topic);
        }
    }

public:
    SensorManager(PubSubClient& mqttClient, DHT& dht)
        : client(mqttClient), dhtSensor(dht) {}

    void readAndPublishSensors() {
        // Read soil moisture
        int rawValue = analogRead(soil_sensor_pin);
        int soilValue = map(rawValue, 0, 1023, 0, 100);
        client.publish(soil_sensor_topic, String(soilValue).c_str());
        Serial.print("Soil Moisture (%): ");
        Serial.println(soilValue);

        // Read temperature
        float temperature = dhtSensor.readTemperature();
        publishSensorData(temperature_topic, temperature);

        // Read humidity
        float humidity = dhtSensor.readHumidity();
        publishSensorData(humidity_topic, humidity);
    }
};

#endif
