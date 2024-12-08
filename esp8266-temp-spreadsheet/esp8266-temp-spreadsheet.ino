#include <ESP8266WiFi.h>
#include <ESP_Google_Sheet_Client.h>
#include "DHT.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

// WiFi credentials
#define WIFI_SSID "your-ssid"
#define WIFI_PASSWORD "your-password"

// Google Sheet credentials
#define PROJECT_ID "your-project-id"
#define CLIENT_EMAIL "your-account.iam.gserviceaccount.com"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\n...\n-----END PRIVATE KEY-----\n";
const char spreadsheetId[] = "your-spreadsheet-id";

// NTP Client
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7 * 3600, 60000); // GMT+7

#define DHTPIN D4 // Pin for the DHT11 sensor
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

unsigned long lastTime = 0;
unsigned long timerDelay = 60; // Delay in seconds

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nConnected to Wi-Fi");

  timeClient.begin();
  timeClient.update();

  GSheet.setTokenCallback([](TokenInfo info) {
    if (info.status == token_status_error) {
      Serial.printf("Token error: %s\n", GSheet.getTokenError(info).c_str());
    }
  });
  GSheet.begin(CLIENT_EMAIL, PROJECT_ID, PRIVATE_KEY);
}

void loop() {
  if (GSheet.ready() && millis() - lastTime > timerDelay * 1000) {
    lastTime = millis();
    timeClient.update();

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Failed to read from DHT sensor");
      return;
    }

    FirebaseJson response;
    FirebaseJson valueRange;
    String timeStampFormula = "=EPOCHTODATE(" + String(timeClient.getEpochTime()) + ")";

    valueRange.add("majorDimension", "COLUMNS");
    valueRange.set("values/[0]/[0]", timeStampFormula);
    valueRange.set("values/[1]/[0]", temperature);
    valueRange.set("values/[2]/[0]", humidity);

    bool success = GSheet.values.append(&response, spreadsheetId, "Sheet1!A1", &valueRange);

    if (success) {
      response.toString(Serial, true);
    } else {
      Serial.println(GSheet.errorReason());
    }

    Serial.printf("Temperature: %.2f °C\n", temperature);
    Serial.printf("Humidity: %.2f %%\n", humidity);
  }
}
