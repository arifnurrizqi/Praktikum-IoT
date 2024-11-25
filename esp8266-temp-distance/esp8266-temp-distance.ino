#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include "DHT.h"

#define relay  D6 // pin of relay for valve in pin 13
#define dht_pin D5 // pin of water flow sensor in pin 12

#define DHTTYPE DHT11   // DHT 11

// Replace with your network credentials
const char* ssid = "";
const char* password = "";

// Replace with your Firebase project credentials
#define FIREBASE_HOST "https://4-default-rtdb.asia-southeast1.firebasedatabase.app/" // isi dengan link firebase database anda
#define FIREBASE_AUTH "database-secret" // isi dengan key database secret anda

FirebaseData firebaseData;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

DHT dht(dht_pin, DHTTYPE);

void setup() {
  Serial.begin(115200);

  dht.begin();

  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);

  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ");
  Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");

  // Print the IP address
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

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
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));

  // rubah data ke string 
  String formatedTemp = String(t, 2);
  String formatedHumi = String(h, 2);

  Firebase.setString(firebaseData, "/realtime/temp", formatedTemp);
  Firebase.setString(firebaseData, "/realtime/humi", formatedHumi);

  Serial.println("Data sent to Firebase");

  delay(500);
}

// Callback function when data on a node changes
void streamCallback(StreamData data) {
  Serial.println("Stream data received...");
  Serial.print("Path: ");
  Serial.println(data.dataPath());
  Serial.print("Data: ");
  Serial.println(data.stringData());
  
  // Jika data diterima, aktifkan atau nonaktifkan valve
  if (data.stringData() == "true") {
    digitalWrite(relay, LOW);
    Serial.println("relay On");
  } else if (data.stringData() == "false") {
    digitalWrite(relay, HIGH);
    Serial.println("relay Off");
  }
}

// Callback function when timeout occurs
void streamTimeoutCallback(bool timeout) {
  if (timeout) {
    Serial.println("Stream timeout, resuming...");
    Firebase.beginStream(firebaseData, "/realtime/relay_status");
  }
}
