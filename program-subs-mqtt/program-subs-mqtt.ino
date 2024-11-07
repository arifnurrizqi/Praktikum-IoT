#include <WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Informasi koneksi WiFi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Koneksi ke broker MQTT
const char* mqtt_server = "broker.hivemq.com";
const char* topic = "arif-iot/test-lamp";

WiFiClient espClient;
PubSubClient client(espClient);

// Konfigurasi NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200, 60000); // GMT+7 (25200 detik)

// Konfigurasi GPIO untuk lampu
const int lampPin = 19;

void setup() {
  Serial.begin(115200);
  pinMode(lampPin, OUTPUT);

  // Koneksi WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  // Set up MQTT
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Inisialisasi NTP
  timeClient.begin();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32Cli-asdasldkas")) {
      Serial.println("connected");
      client.subscribe(topic);  // Ganti dengan topik yang sesuai
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// Fungsi callback untuk menangani pesan yang diterima
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Ambil waktu saat pesan diterima
  timeClient.update();
  String formattedTime = timeClient.getFormattedTime();
  Serial.print("Time received: ");
  Serial.println(formattedTime);

  // Periksa pesan dan kontrol lampu
  if (messageTemp == "1") {
    digitalWrite(lampPin, HIGH);  // Nyalakan lampu
    Serial.println("Lamp ON");
  } else if (messageTemp == "0") {
    digitalWrite(lampPin, LOW);   // Matikan lampu
    Serial.println("Lamp OFF");
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  timeClient.update();
}
