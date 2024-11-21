// Blynk Configuration
#define BLYNK_TEMPLATE_ID  "template_id_here"
#define BLYNK_TEMPLATE_NAME "template_name_here"
#define BLYNK_AUTH_TOKEN    "auth_token_here"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <BlynkSimpleEsp8266.h>

#define DHT_PIN D4 // Pin GPIO untuk sensor, sesuaikan dengan ESP8266

const char* ssid     = "your_wifi_ssid";
const char* password = "your_wifi_password";

char blynkAuthToken[34];

WiFiClient espClient;
DHT dht(DHT_PIN, DHT11);
float temp = 0;
float humi = 0;

// Blynk setup
BlynkTimer timer;

void sendToBlynk(float temp, float humi) {
  Blynk.virtualWrite(V0, temp);
  Blynk.virtualWrite(V1, humi);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  // Tunggu hingga koneksi berhasil
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  dht.begin();

  // Konfigurasi Blynk
  Blynk.config(blynkAuthToken);

  // Tambahkan pengecekan koneksi ke server Blynk
  if (!Blynk.connect()) {
    Serial.println("Blynk failed to connect. Check your credentials.");
  }

  // Timer untuk membaca sensor
  timer.setInterval(1000L, []() {
    temp = dht.readTemperature();
    humi = dht.readHumidity();

    if (isnan(temp) || isnan(humi)) {
      Serial.println("Failed to read from DHT sensor!");
      return; // Jika pembacaan gagal, hentikan eksekusi callback
    }

    Serial.print("Temperature: ");
    Serial.println(temp);
    Serial.print("Humidity: ");
    Serial.println(humi);

    Blynk.logEvent("gas_safe_alarm", "AMAN! Kadar CO atau CO2 RENDAH");

    if (temp > 35.0 ) {
      Serial.println("Status: Panas");
      Blynk.logEvent("temperature_alarm", "Suhu terlalu panas");
    }
    
    if (humi > 80.0) {
      Serial.println("Status: Lembab - Ruangan Terlalu Lembab");
