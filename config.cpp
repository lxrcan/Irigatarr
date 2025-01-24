// Config.cpp
#include "Config.h"

// Wi-Fi Configuration
const char* ssid = "rs500m-19a9b1-1";
const char* password = "978dd720f969c";

// Plant Configuration
const char* plant_name = "Monstera1"; // Define the plant name

// MQTT Configuration
const char* mqtt_server = "192.168.1.46";
const int mqtt_port = 1883;
const char* soil_sensor_topic = "home/soil_moisture";
const char* temperature_topic = "home/temperature";
const char* humidity_topic = "home/humidity";
const char* pump_control_topic = "home/control_pump";
const char* status_topic = "home/device_status";
const char* discovery_topic = "homeassistant/device_discovery";

// Pin Configuration
const int pump_pin = 5; // GPIO 5 (D1)
const int soil_sensor_pin = 0; // Analog pin
const int dht_pin = 2; // GPIO number for D4

