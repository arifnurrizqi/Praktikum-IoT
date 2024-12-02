#include <WiFi.h>
#include <FirebaseESP32.h>
#include "DHT.h"

// Konfigurasi Firebase
#define FIREBASE_HOST "https://default-rtdb.asia-southeast1.firebasedatabase.app/" // Ganti dengan link Firebase Anda
#define FIREBASE_AUTH "secret-database" // Ganti dengan database secret Anda

// Konfigurasi WiFi
const char* ssid = "your-ssid";          // Ganti dengan SSID WiFi Anda
const char* password = "your-passw"; // Ganti dengan password WiFi Anda

// Konfigurasi DHT
#define DHTPIN 17       // Pin DHT11
#define DHTTYPE DHT11   // Jenis sensor DHT
DHT dht(DHTPIN, DHTTYPE);

// Konfigurasi Relay
#define RELAY_PIN 18
FirebaseData firebaseData; // Objek Firebase

// Variabel untuk interval waktu
unsigned long previousMillis = 0; // Penyimpan waktu terakhir
const unsigned long interval = 5000; // Interval 5 detik

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Pastikan relay mati saat awal

  // Koneksi ke WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");

  // Inisialisasi Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  // Streaming data dari Firebase untuk relay_status
  if (Firebase.beginStream(firebaseData, "/realtime/relay_status")) {
    Serial.println("Firebase stream started...");
  } else {
    Serial.print("Could not start stream: ");
    Serial.println(firebaseData.errorReason());
  }

  // Callback untuk menangani stream data
  Firebase.setStreamCallback(firebaseData, streamCallback, streamTimeoutCallback);
}

void loop() {
  unsigned long currentMillis = millis();

  // Cek apakah sudah mencapai interval waktu 5 detik
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; // Update waktu terakhir

    // Membaca nilai dari sensor DHT11
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    // Validasi pembacaan sensor
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // Menampilkan data di Serial Monitor
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print("% Temperature: ");
    Serial.print(t);
    Serial.println("°C");

    // Kirim data ke Firebase
    if (Firebase.setFloat(firebaseData, "/realtime/humi", h)) {
      Serial.println("Humidity sent to Firebase");
    } else {
      Serial.print("Failed to send humidity: ");
      Serial.println(firebaseData.errorReason());
    }

    if (Firebase.setFloat(firebaseData, "/realtime/temp", t)) {
      Serial.println("Temperature sent to Firebase");
    } else {
      Serial.print("Failed to send temperature: ");
      Serial.println(firebaseData.errorReason());
    }
  }

  // Firebase stream akan tetap berjalan tanpa mengganggu interval
}

// Callback untuk menangani perubahan data dari Firebase
void streamCallback(StreamData data) {
  Serial.println("Stream data received...");
  Serial.print("Path: ");
  Serial.println(data.dataPath());
  Serial.print("Data: ");
  Serial.println(data.stringData());

  // Kontrol relay berdasarkan data dari Firebase
  if (data.stringData() == "true") {
    digitalWrite(RELAY_PIN, HIGH); // Relay aktif
    Serial.println("Relay ON");
  } else if (data.stringData() == "false") {
    digitalWrite(RELAY_PIN, LOW); // Relay mati
    Serial.println("Relay OFF");
  }
}

// Callback untuk timeout stream
void streamTimeoutCallback(bool timeout) {
  if (timeout) {
    Serial.println("Stream timeout, resuming...");
    Firebase.beginStream(firebaseData, "/realtime/relay_status");
  }
}
