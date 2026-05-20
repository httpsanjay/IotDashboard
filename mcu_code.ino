#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>

// ---------------- WiFi Credentials ----------------
const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";

// ---------------- MQTT Configuration ----------------
const char* mqtt_server = "mqtt.example.com";
const int mqtt_port = 8883;
const char* mqtt_user = "mqtt_user";
const char* mqtt_pass = "mqtt_pass";

// ---------------- MQTT Topics ----------------
const char* topic_temp        = "esp32/bmp280/temperature";
const char* topic_pressure    = "esp32/bmp280/pressure";
const char* topic_altitude    = "esp32/bmp280/altitude";
const char* topic_waterlevel  = "esp32/waterlevel";
const char* topic_status      = "esp32/sensorstatus";

// ---------------- Hardware ----------------
const int waterPin = 34;

WiFiClientSecure espClient;
PubSubClient client(espClient);
Adafruit_BMP280 bmp;

bool sensor_ok = false;

// ---------------- WiFi Setup ----------------
void setup_wifi() {

  Serial.print("Connecting to WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");
}

// ---------------- MQTT Reconnect ----------------
void reconnect() {

  while (!client.connected()) {

    Serial.print("Connecting to MQTT...");

    String clientID = "ESP32Client-";
    clientID += String(random(0xffff), HEX);

    if (client.connect(clientID.c_str(), mqtt_user, mqtt_pass)) {

      Serial.println("Connected");

    } else {

      Serial.print("Failed, rc=");
      Serial.print(client.state());

      Serial.println(" retrying in 2 seconds");

      delay(2000);
    }
  }
}

// ---------------- Setup ----------------
void setup() {

  Serial.begin(115200);

  pinMode(waterPin, INPUT);

  Wire.begin();

  setup_wifi();

  // TLS Connection
  espClient.setInsecure();

  client.setServer(mqtt_server, mqtt_port);

  // BMP280 Detection
  if (bmp.begin(0x76)) {

    sensor_ok = true;
    Serial.println("BMP280 Found at 0x76");

  } else if (bmp.begin(0x77)) {

    sensor_ok = true;
    Serial.println("BMP280 Found at 0x77");

  } else {

    sensor_ok = false;
    Serial.println("BMP280 NOT FOUND");
  }
}

// ---------------- Main Loop ----------------
void loop() {

  if (!client.connected()) {
    reconnect();
  }

  client.loop();



  // Read Sensors
  float temperature = bmp.readTemperature();

  float pressure = bmp.readPressure() / 100.0;

  float altitude = bmp.readAltitude(1013.25);

  int waterRaw = analogRead(waterPin);

  float waterPercent = map(waterRaw, 0, 4095, 0, 100);

  // Validate Readings
  if (
      isnan(temperature) ||
      isnan(pressure) ||
      isnan(altitude) ||
      temperature < -40 ||
      temperature > 85 ||
      pressure < 300 ||
      pressure > 1100
     ) {

      client.publish(topic_status, "0");

      Serial.println("Invalid Sensor Reading");

      delay(3000);

      return;
  }

  // Publish Sensor Data
  client.publish(topic_temp, String(temperature, 2).c_str());

  client.publish(topic_pressure, String(pressure, 2).c_str());

  client.publish(topic_altitude, String(altitude, 2).c_str());

  client.publish(topic_waterlevel, String(waterPercent, 2).c_str());

  client.publish(topic_status, "1");

  // Serial Monitor Output
  Serial.println("Published Sensor Data");

  Serial.print("Temperature: ");
  Serial.println(temperature);

  Serial.print("Pressure: ");
  Serial.println(pressure);

  Serial.print("Altitude: ");
  Serial.println(altitude);

  Serial.print("Water Level: ");
  Serial.println(waterPercent);

  delay(3000);
}
