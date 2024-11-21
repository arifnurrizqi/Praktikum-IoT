// Blynk Configuration
#define BLYNK_TEMPLATE_ID  "template_id_here"
#define BLYNK_TEMPLATE_NAME "template_name_here"
#define BLYNK_AUTH_TOKEN    "auth_token_here"

#include <Arduino.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <BlynkSimpleEsp32.h> // Library for Blynk IoT

#define DHT_PIN 21 // Pin GPIO untuk sensor, sesuaikan dengan pin ESP32 Anda

const char* ssid     = "your_wifi_ssid"; // WiFi SSID
const char* password = "your_wifi_password"; // WiFi Password

DHT dht(DHT_PIN, DHT11);
float temp = 0;
float humi = 0;

// Blynk setup
BlynkTimer timer;

void sendToBlynk(float temp, float humi) {
  Blynk.virtualWrite(V0, temp); // Kirim data suhu ke Blynk
  Blynk.virtualWrite(V1, humi); // Kirim data kelembapan ke Blynk
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  // Tunggu koneksi WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);

  setup_wifi();
  dht.begin();

  // Konfigurasi Blynk
  Blynk.config(BLYNK_AUTH_TOKEN);

  // Coba koneksi ke server Blynk
  if (!Blynk.connect()) {
    Serial.println("Failed to connect to Blynk. Check your token and connection.");
  }

  // Timer untuk membaca data setiap 1 detik
  timer.setInterval(1000L, []() {
    temp = dht.readTemperature();
    humi = dht.readHumidity();

    if (isnan(temp) || isnan(humi)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    Serial.print("Temperature: ");
    Serial.println(temp);
    Serial.print("Humidity: ");
    Serial.println(humi);

    Blynk.logEvent("gas_safe_alarm", "AMAN! Kadar CO atau CO2 RENDAH");

    if (temp > 35.0) {
      Serial.println("Status: Panas");
      Blynk.logEvent("temperature_alarm", "Suhu terlalu panas");
    }

    if (humi > 80.0) {
      Serial.println("Status: Lembab - Ruangan Terlalu Lembab");
      Blynk.logEvent("humidity_alarm", "Ruangan Terlalu lembab, buka jendela!");
    }

    // Kirim data ke Blynk
    sendToBlynk(temp, humi);
  });
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected.
