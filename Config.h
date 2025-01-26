#ifndef CONFIG_H
#define CONFIG_H

// ===============================
// System Configuration (Do Not Edit)
// ===============================

// Wi-Fi Configuration
extern const char* ssid;          // Wi-Fi SSID
extern const char* password;      // Wi-Fi Password

// MQTT Configuration
extern const char* mqtt_server;   // MQTT Broker IP Address
extern const int mqtt_port;       // MQTT Broker Port
extern const char* discovery_topic; // MQTT Discovery Topic (for Home Assistant)

// Pin Configuration
extern const int pump_pin;        // GPIO Pin for Pump Control
extern const int soil_sensor_pin; // Analog Pin for Soil Moisture Sensor
extern const int dht_pin;         // GPIO Pin for DHT Sensor

// ===============================
// User Preferences
// ===============================

// Plant Configuration
extern const char* plant_name;    // Name of the plant for identification

// MQTT Topics
extern const char* soil_sensor_topic;    // MQTT Topic for Soil Moisture Data
extern const char* temperature_topic;    // MQTT Topic for Temperature Data
extern const char* humidity_topic;       // MQTT Topic for Humidity Data
extern const char* pump_control_topic;   // MQTT Topic for Pump Control Commands
extern const char* status_topic;         // MQTT Topic for Device Status

// Pump Control Preferences
extern unsigned long cooldown_period;    // Cooldown period after a pump cycle in ms (0 to disable)
extern unsigned long max_cycle_time;     // Maximum pump cycle time in ms
extern unsigned long pump_on_duration;   // Pump ON duration per cycle in ms
extern unsigned long pump_off_duration;  // Pump OFF duration per cycle in ms

#endif // CONFIG_H
