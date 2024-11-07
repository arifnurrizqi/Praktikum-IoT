#include <Arduino.h>

#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define DHT_PIN 21 // Pin GPIO untuk sensor DS18B20
#define TRIGGER_PIN 23 // Pin GPIO untuk trigger HC-SR04
#define ECHO_PIN 22 // Pin GPIO untuk echo HC-SR04
#define MAX_DISTANCE 400 // Jarak maksimum yang akan diukur oleh sensor HC-SR04
#define INTERVAL 5000 // Interval pengiriman data ke server MQTT (ms)

const char* ssid     = "Wokwi-GUEST"; //your wifi ssid
const char* password = ""; //your wifi password
const char* mqtt_server = "broker.hivemq.com";

const char* temperature_topic = "esp32-ariff/temp";
const char* distance_topic = "esp32-ariff/distance";

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHT_PIN, DHT22);
long lastMsg = 0;
float temperature = 0;
float distance = 0;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("clientId-xEZb7frpLH")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  dht.begin();
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  if (now - lastMsg > INTERVAL) {
    lastMsg = now;
    temperature = dht.readTemperature();
    Serial.print("Temperature: ");
    Serial.println(temperature);
    client.publish(temperature_topic, String(temperature).c_str());
    
    float duration, distance;
    digitalWrite(TRIGGER_PIN, LOW);  
    delayMicroseconds(2); 
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10); 
    digitalWrite(TRIGGER_PIN, LOW);
    duration = pulseIn(ECHO_PIN, HIGH);
    distance = (duration / 2) * 0.0343;
    Serial.print("distance: ");
    Serial.println(distance);
    client.publish(distance_topic, String(distance).c_str());
  }
  delay(100);
}
