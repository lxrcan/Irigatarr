#include "Config.h"

// ===============================
// System Configuration
// ===============================

// Wi-Fi Configuration
const char* ssid = "rs500m-19a9b1-1";          // Replace with your Wi-Fi SSID
const char* password = "978dd720f969c";        // Wi-Fi Password

// MQTT Configuration
const char* mqtt_server = "192.168.1.46";  // Replace with your MQTT Broker IP Address
const int mqtt_port = 1883;                // Default MQTT Broker Port
const char* discovery_topic = "homeassistant/device_discovery"; // MQTT Discovery Topic

// Pin Configuration
const int pump_pin = 5;        // GPIO Pin for Pump Control
const int soil_sensor_pin = 0; // Analog Pin for Soil Moisture Sensor
const int dht_pin = 2;         // GPIO Pin for DHT Sensor

// ===============================
// User Preferences
// ===============================

// Plant Configuration
const char* plant_name = "Monstera1";  // Name of the plant for identification

// MQTT Topics
const char* soil_sensor_topic = "home/soil_moisture";    // Soil Moisture Data Topic
const char* temperature_topic = "home/temperature";      // Temperature Data Topic
const char* humidity_topic = "home/humidity";            // Humidity Data Topic
const char* pump_control_topic = "home/control_pump";    // Pump Control Commands Topic
const char* status_topic = "home/device_status";         // Device Status Topic

// Pump Control Preferences
unsigned long cooldown_period = 600000;  // Cooldown period after a pump cycle in ms (10 minutes, 0 to disable)
unsigned long max_cycle_time = 3600000;  // Maximum pump cycle time in ms (1 hour)
unsigned long pump_on_duration = 300000; // Pump ON duration per cycle in ms (5 minutes)
unsigned long pump_off_duration = 600000; // Pump OFF duration per cycle in ms (10 minutes)
