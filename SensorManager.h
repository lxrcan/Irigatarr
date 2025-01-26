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

    void publishSensorData(const char* topic, float value, int decimalPlaces = 2) {
        char formattedValue[10];
        dtostrf(value, 1, decimalPlaces, formattedValue);
        client.publish(topic, formattedValue);
        Serial.print(topic);
        Serial.print(": ");
        Serial.println(formattedValue);
    }

public:
    SensorManager(PubSubClient& mqttClient, DHT& dht)
        : client(mqttClient), dhtSensor(dht) {}

    void readAndPublishSensors() {
        // Read soil moisture
        int rawValue = analogRead(soil_sensor_pin);
        float soilMoisture = map(rawValue, 0, 1023, 0, 100);
        publishSensorData(soil_sensor_topic, soilMoisture, 0);

        // Read temperature
        float temperature = dhtSensor.readTemperature();
        publishSensorData(temperature_topic, temperature);

        // Read humidity
        float humidity = dhtSensor.readHumidity();
        publishSensorData(humidity_topic, humidity);
    }
};

#endif
