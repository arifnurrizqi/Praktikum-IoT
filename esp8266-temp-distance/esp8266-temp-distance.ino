#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include "DHT.h"

#define relay  D2 // pin of relay for valve in pin 13
#define dht_pin D5 // pin of dht sensor in pin 12

#define DHTTYPE DHT11   // DHT 11

// Replace with your network credentials
const char* ssid = "your-ssid";
const char* password = "your-ssid-pssw";

// Replace with your Firebase project credentials
#define FIREBASE_HOST "https://xxxxdefault-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "your-database-secret"

FirebaseData firebaseData;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

DHT dht(dht_pin, DHTTYPE);

unsigned long previousMillis = 0;
const long interval = 5000;  // Interval untuk pembacaan suhu dan kelembapan setiap 5 detik

void setup() {
  Serial.begin(115200);

  dht.begin();

  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);

  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");

  // Initialize Firebase connection
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;

  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);

  // Starting Firebase Streaming to get data reset status
  if (Firebase.beginStream(firebaseData, "/realtime/relay_status")) {
    Serial.println("Firebase stream started...");
  } else {
    Serial.print("Could not start stream: ");
    Serial.println(firebaseData.errorReason());
  }

  // Set callback functions for event stream
  Firebase.setStreamCallback(firebaseData, streamCallback, streamTimeoutCallback);
}

void loop() {
  unsigned long currentMillis = millis();

  // Cek jika interval waktu telah tercapai
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Reading temperature or humidity
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
    Serial.println(F("°C "));

    // Rubah data ke string
    String formatedTemp = String(t, 2);
    String formatedHumi = String(h, 2);

    // Kirim data ke Firebase
    Firebase.setString(firebaseData, "/realtime/temp", formatedTemp);
    Firebase.setString(firebaseData, "/realtime/humi", formatedHumi);

    Serial.println("Data sent to Firebase");
  }

  // Loop tidak terhalang oleh delay
  // Kontrol relay tetap berjalan berdasarkan stream Firebase
}

// Callback function when data on a node changes
void streamCallback(StreamData data) {
  Serial.println("Stream data received...");
  Serial.print("Path: ");
  Serial.println(data.dataPath());
  Serial.print("Data: ");
  Serial.println(data.stringData());

  // Jika data diterima, aktifkan atau nonaktifkan relay
  if (data.stringData() == "true") {
    digitalWrite(relay, LOW);  // Relay ON
    Serial.println("Relay ON");
  } else if (data.stringData() == "false") {
    digitalWrite(relay, HIGH); // Relay OFF
    Serial.println("Relay OFF");
  }
}

// Callback function when timeout occurs
void streamTimeoutCallback(bool timeout) {
  if (timeout) {
    Serial.println("Stream timeout, resuming...");
    Firebase.beginStream(firebaseData, "/realtime/relay_status");
  }
}
