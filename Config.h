#ifndef CONFIG_H
#define CONFIG_H

// Wi-Fi Configuration
extern const char* ssid;
extern const char* password;

// MQTT Configuration
extern const char* mqtt_server;
extern const int mqtt_port;
extern const char* soil_sensor_topic;
extern const char* pump_control_topic;
extern const char* status_topic;
extern const char* discovery_topic;
extern const char* temperature_topic;
extern const char* humidity_topic;

// Pin Configuration
extern const int pump_pin;
extern const int soil_sensor_pin;
extern const int dht_pin;

// Plant Configuration
extern const char* plant_name;

#endif // CONFIG_H
