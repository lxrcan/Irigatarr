#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h" // Include the DHT library for temperature and humidity

#include "Config.h"

WiFiClient espClient;
PubSubClient client(espClient);

#define DHTPIN D4  // Define the pin for the DHT sensor
#define DHTTYPE DHT11 // Change to DHT22 if you’re using a DHT22 sensor

DHT dht(DHTPIN, DHTTYPE);

unsigned long lastPumpActivation = 0;
bool isPumpRunning = false;
const unsigned long maxPumpRunTime = 20000; // 20 seconds

void connectToWiFi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
}

void connectToMQTT() {
    while (!client.connected()) {
        Serial.print("Connecting to MQTT...");
        if (client.connect("plant_name")) {
            Serial.println("connected");

            // Publish availability
            client.publish(status_topic, "online", true);

            // Subscribe to pump control topic
            client.subscribe(pump_control_topic);

            // Publish discovery message for soil moisture
            String soilDiscoveryPayload = "{\"device\":{\"identifiers\":[\"" + String(plant_name) + "\"],\"model\":\"ESP8266\",\"manufacturer\":\"Custom\"},\"availability_topic\":\"" + String(status_topic) + "\",\"state_topic\":\"" + String(soil_sensor_topic) + "\",\"unit_of_measurement\":\"%\",\"unique_id\":\"soil_sensor_" + String(plant_name) + "\",\"device_class\":\"moisture\"}";
            client.publish("homeassistant/sensor/soil_moisture/config", soilDiscoveryPayload.c_str(), true);

            // Publish discovery message for temperature
            String tempDiscoveryPayload = "{\"device\":{\"identifiers\":[\"" + String(plant_name) + "\"],\"model\":\"ESP8266\",\"manufacturer\":\"Custom\"},\"availability_topic\":\"" + String(status_topic) + "\",\"state_topic\":\"" + String(temperature_topic) + "\",\"unit_of_measurement\":\"°C\",\"unique_id\":\"temperature_sensor_" + String(plant_name) + "\",\"device_class\":\"temperature\"}";
            client.publish("homeassistant/sensor/temperature/config", tempDiscoveryPayload.c_str(), true);

            // Publish discovery message for humidity
            String humidityDiscoveryPayload = "{\"device\":{\"identifiers\":[\"" + String(plant_name) + "\"],\"model\":\"ESP8266\",\"manufacturer\":\"Custom\"},\"availability_topic\":\"" + String(status_topic) + "\",\"state_topic\":\"" + String(humidity_topic) + "\",\"unit_of_measurement\":\"%\",\"unique_id\":\"humidity_sensor_" + String(plant_name) + "\",\"device_class\":\"humidity\"}";
            client.publish("homeassistant/sensor/humidity/config", humidityDiscoveryPayload.c_str(), true);

        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}


void mqttCallback(char* topic, byte* payload, unsigned int length) {
    String message = "";
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }

    Serial.print("MQTT message received on topic: ");
    Serial.println(topic);
    Serial.print("Message: ");
    Serial.println(message);

    pumpControl.handleMessage(topic, message); // Delegate pump-related messages to PumpControl
}

void setup() {
    Serial.begin(115200);

    // Set up pins
    pinMode(pump_pin, OUTPUT);
    digitalWrite(pump_pin, LOW);

    // Initialize DHT sensor
    dht.begin();

    // Set up WiFi and MQTT
    connectToWiFi();
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(mqttCallback);

    // Set MQTT buffer size
    client.setBufferSize(1024); // Adjust size as needed (e.g., 1024 or 2048 bytes)

    // Initialize PumpControl
    pumpControl.init();

}

void loop() {
    // Maintain MQTT connection
    if (!client.connected()) {
        connectToMQTT();
    }
    client.loop();

    // Read soil sensor data
    int rawValue = analogRead(soil_sensor_pin);
    int soilValue = map(rawValue, 0, 1023, 0, 100);
    String soilMessage = String(soilValue);

    Serial.print("Soil sensor value: ");
    Serial.println(soilValue);
    client.publish(soil_sensor_topic, soilMessage.c_str());

    // Read temperature and humidity from DHT sensor
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Failed to read from DHT sensor!");
    } else {
        String tempMessage = String(temperature);
        String humidityMessage = String(humidity);

        Serial.print("Temperature: ");
        Serial.println(tempMessage);
        Serial.print("Humidity: ");
        Serial.println(humidityMessage);

        client.publish(temperature_topic, tempMessage.c_str());
        client.publish(humidity_topic, humidityMessage.c_str());
    }

    // Handle pump timeout
      pumpControl.checkTimeout();


    delay(3000); // Adjust as needed
}
