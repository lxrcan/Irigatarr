// Simple Irrigation System

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// Configurations
const char* ssid = "rs500m-19a9b1-1";
const char* password = "978dd720f969c";
const char* mqtt_server = "192.168.1.46";
const int mqtt_port = 1883;

const char* soil_sensor_topic = "home/soil_moisture";
const char* temperature_topic = "home/temperature";
const char* humidity_topic = "home/humidity";
const char* pump_control_topic = "home/control_pump";
const char* status_topic = "home/device_status";
const char* discovery_topic = "homeassistant/device_discovery";

const int pump_pin = 5;
const int soil_sensor_pin = A0;
const int dht_pin = 2;

const unsigned long cooldown_period = 600000; // 10 minutes
const unsigned long max_cycle_time = 60000;   // 1 minute
const unsigned long pump_on_duration = 6000;  // 6 seconds
const unsigned long pump_off_duration = 6000; // 6 seconds

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(dht_pin, DHT22);

bool isRunning = false;
unsigned long cycleStartTime = 0;
unsigned long lastStopTime = 0;
unsigned long nextActionTime = 0;

void connectWiFi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("WiFi connected.");
}

void connectMQTT() {
    while (!client.connected()) {
        if (client.connect("IrrigationSystem")) {
            Serial.println("Connected to MQTT broker.");
            client.subscribe(pump_control_topic);
            publishDiscoveryPayload();
        } else {
            delay(5000);
            Serial.println("Retrying MQTT connection...");
        }
    }
}

void publishDiscoveryPayload() {
    String payload = "{\"name\":\"IrrigationSystem\",\"state_topic\":\"";
    payload += status_topic;
    payload += "\",\"command_topic\":\"";
    payload += pump_control_topic;
    payload += "\",\"sensors\":{\"soil_moisture\":\"";
    payload += soil_sensor_topic;
    payload += "\",\"temperature\":\"";
    payload += temperature_topic;
    payload += "\",\"humidity\":\"";
    payload += humidity_topic;
    payload += "\"}}";
    client.publish(discovery_topic, payload.c_str(), true);
    Serial.println("Published discovery payload:");
    Serial.println(payload);
}

void publishSensorData(const char* topic, float value, int decimalPlaces = 2) {
    char buffer[10];
    dtostrf(value, 1, decimalPlaces, buffer);
    client.publish(topic, buffer);
}

void readAndPublishSensors() {
    int rawValue = analogRead(soil_sensor_pin);
    float soilMoisture = map(rawValue, 0, 1023, 0, 100);
    publishSensorData(soil_sensor_topic, soilMoisture, 0);

    float temperature = dht.readTemperature();
    publishSensorData(temperature_topic, temperature);

    float humidity = dht.readHumidity();
    publishSensorData(humidity_topic, humidity);
}

void handlePumpCommand(char* topic, byte* payload, unsigned int length) {
    String command((char*)payload);
    unsigned long now = millis();

    if (command == "ON" && now - lastStopTime >= cooldown_period) {
        isRunning = true;
        cycleStartTime = millis();
        nextActionTime = millis() + pump_on_duration;
        digitalWrite(pump_pin, HIGH);
        client.publish(status_topic, "Pump cycle started.");
    } else if (command == "OFF") {
        isRunning = false;
        digitalWrite(pump_pin, LOW);
        client.publish(status_topic, "Pump stopped.");
    }
}

void managePumpCycle() {
    if (!isRunning) return;

    unsigned long now = millis();

    if (now - cycleStartTime >= max_cycle_time) {
        isRunning = false;
        digitalWrite(pump_pin, LOW);
        lastStopTime = millis();
        client.publish(status_topic, "Pump cycle ended: Max cycle time reached.");
        return;
    }

    if (digitalRead(pump_pin) == HIGH && now >= nextActionTime) {
        digitalWrite(pump_pin, LOW);
        nextActionTime = now + pump_off_duration;
        client.publish(status_topic, "Pump OFF: Rest period.");
    } else if (digitalRead(pump_pin) == LOW && now >= nextActionTime) {
        digitalWrite(pump_pin, HIGH);
        nextActionTime = now + pump_on_duration;
        client.publish(status_topic, "Pump ON: Watering.");
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(pump_pin, OUTPUT);
    digitalWrite(pump_pin, LOW);

    connectWiFi();
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(handlePumpCommand);

    dht.begin();
}

void loop() {
    if (!client.connected()) connectMQTT();
    client.loop();

    readAndPublishSensors();
    managePumpCycle();
    delay(1000); // Adjust delay for sensor readings
}
