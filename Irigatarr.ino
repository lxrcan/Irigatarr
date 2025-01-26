  #include <ESP8266WiFi.h>
  #include <PubSubClient.h>
  #include "DHT.h"
  #include "Config.h"
  #include "SensorManager.h"
  #include "PumpController.h"

  // Create instances for WiFi and MQTT
  WiFiClient espClient;
  PubSubClient client(espClient);

  // Create DHT sensor instance
  DHT dht(dht_pin, DHT11); // Replace DHT11 with DHT22 if needed

  // Create instances of SensorManager and PumpController
  PumpController pumpController(client, pump_pin);
  SensorManager sensorManager(client, dht);

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
          if (client.connect("ESP8266_PumpController")) {
              Serial.println("connected");

              // Subscribe to pump control commands
              client.subscribe(pump_control_topic);

              // Publish initial status
              client.publish(status_topic, "online", true);
          } else {
              Serial.print("failed, rc=");
              Serial.print(client.state());
              Serial.println(" retrying in 5 seconds...");
              delay(5000);
          }
      }
  }

  // MQTT Callback
  void mqttCallback(char* topic, byte* payload, unsigned int length) {
      String message = "";
      for (unsigned int i = 0; i < length; i++) {
          message += (char)payload[i];
      }

      if (String(topic) == pump_control_topic) {
          pumpController.handlePumpCommand(message.c_str());
      }
  }

  void setup() {
      Serial.begin(115200);

      // Initialize GPIO pins
      pinMode(pump_pin, OUTPUT);
      digitalWrite(pump_pin, LOW);
      Serial.println("Pump set to LOW (OFF) at startup.");

      // Initialize DHT sensor
      dht.begin();

      // Connect to WiFi and MQTT
      connectToWiFi();
      client.setServer(mqtt_server, mqtt_port);
      client.setCallback(mqttCallback);

      // Publish retained OFF state for the pump control topic
      client.publish(pump_control_topic, "OFF", true); // Retained = true
      Serial.println("Published 'OFF' to pump control topic at startup.");
  }

  void loop() {
      // Ensure MQTT connection
      if (!client.connected()) {
          connectToMQTT();
      }
      client.loop();

      // Read and publish sensor data
      sensorManager.readAndPublishSensors();

      // Manage pump cycle
      pumpController.manageCycle();

      // Delay for stability
      delay(2000);
  }
