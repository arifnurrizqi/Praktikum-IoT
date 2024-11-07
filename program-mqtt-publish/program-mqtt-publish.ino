#include <WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Informasi koneksi WiFi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Koneksi ke broker MQTT
const char* mqtt_server = "broker.hivemq.com";

// Topic MQTT
const char* topic = "arif-iot/test-lamp";

WiFiClient espClient;
PubSubClient client(espClient);

// Konfigurasi NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7 * 3600, 60000);  // GMT+7

// Konfigurasi tombol
const int buttonPin = 19;
bool lastButtonState = LOW;
bool currentButtonState;
int lampState = 0;  // Variabel yang bergantian antara 0 dan 1

// Client ID MQTT unik
String clientId;

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLDOWN);

  // Koneksi WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  // Generate client ID unik berdasarkan alamat MAC
  clientId = "ESP32Client-" + WiFi.macAddress();
  client.setServer(mqtt_server, 1883);

  // Set up NTP
  timeClient.begin();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  timeClient.update();

  currentButtonState = digitalRead(buttonPin);

  // Deteksi perubahan dari LOW ke HIGH (tekanan tombol)
  if (currentButtonState == HIGH && lastButtonState == LOW) {
    delay(100);  // Debouncing tombol
    lampState = !lampState;  // Toggle antara 0 dan 1

    // Menampilkan waktu dan nilai ke Serial Monitor
    String formattedTime = timeClient.getFormattedTime();
    Serial.print("Time: ");
    Serial.print(formattedTime);
    Serial.print(" | lamp: ");
    Serial.println(lampState);

    // Mengirim nilai lampState ke topik MQTT
    client.publish(topic, String(lampState).c_str());
  }

  // Update status tombol terakhir
  lastButtonState = currentButtonState;

  delay(50);  // Jeda untuk menghindari pembacaan yang terlalu cepat
}
