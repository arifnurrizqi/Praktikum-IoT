#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "DHT.h"

// Definisi pin
#define relay 18  // Pin relay
#define dht_pin 17 // Pin DHT

#define DHTTYPE DHT11 // Tipe DHT

// Kredensial Wi-Fi
const char* ssid = "";
const char* password = "";

// Kredensial Firebase
#define FIREBASE_HOST "https://default-rtdb.asia-southeast1.firebasedatabase.app/" // Ganti dengan Firebase Host Anda
#define FIREBASE_AUTH "database-secret" // Ganti dengan Firebase Secret Anda

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

DHT dht(dht_pin, DHTTYPE);

void setup() {
  Serial.begin(115200);

  dht.begin();

  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);

  // Koneksi Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ");
  Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");

  // Print alamat IP
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Konfigurasi Firebase
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Memulai streaming data dari Firebase
  if (Firebase.RTDB.beginStream(&fbdo, "/realtime/relay_status")) {
    Serial.println("Firebase stream started...");
  } else {
    Serial.print("Could not start stream: ");
    Serial.println(fbdo.errorReason());
  }

  // Callback untuk data stream
  Firebase.RTDB.setStreamCallback(&fbdo, streamCallback, streamTimeoutCallback);
}

void loop() {
  // Membaca data suhu dan kelembapan
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C"));

  // Konversi data ke string
  String formatedTemp = String(t, 2);
  String formatedHumi = String(h, 2);

  // Mengirim data ke Firebase
  if (Firebase.RTDB.setString(&fbdo, "/realtime/temp", formatedTemp)) {
    Serial.println("Temperature sent to Firebase");
  } else {
    Serial.print("Error sending temp: ");
    Serial.println(fbdo.errorReason());
  }

  if (Firebase.RTDB.setString(&fbdo, "/realtime/humi", formatedHumi)) {
    Serial.println("Humidity sent to Firebase");
  } else {
    Serial.print("Error sending humi: ");
    Serial.println(fbdo.errorReason());
  }

  delay(500);
}

// Callback untuk data stream
void streamCallback(FirebaseStream data) {
  Serial.println("Stream data received...");
  Serial.print("Path: ");
  Serial.println(data.dataPath());
  Serial.print("Data: ");
  Serial.println(data.stringData());

  // Kontrol relay
  if (data.stringData() == "true") {
    digitalWrite(relay, LOW);
    Serial.println("Relay ON");
  } else if (data.stringData() == "false") {
    digitalWrite(relay, HIGH);
    Serial.println("Relay OFF");
  }
}

// Callback untuk timeout streaming
void streamTimeoutCallback(bool timeout) {
  if (timeout) {
    Serial.println("Stream timeout, resuming...");
    Firebase.RTDB.beginStream(&fbdo, "/realtime/relay_status");
  }
}
