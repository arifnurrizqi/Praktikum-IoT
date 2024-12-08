#include <WiFi.h>
#include "time.h"
#include <ESP_Google_Sheet_Client.h>
#include "DHT.h"

#define WIFI_SSID "your-ssid"
#define WIFI_PASSWORD "your-password"

// Google Project ID
#define PROJECT_ID "your-project-id"

// Service Account's client email
#define CLIENT_EMAIL "your-account.iam.gserviceaccount.com"

// Service Account's private key
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\n...\n-----END PRIVATE KEY-----\n";

// The ID of the spreadsheet where you'll publish the data
const char spreadsheetId[] = "your-spreadsheet-id";

#define DHTPIN 5 // Pin for the DHT11 sensor
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 60; // Delay in seconds between data logs

// NTP server to request epoch time
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7 * 3600; // GMT+7 offset in seconds
const int daylightOffset_sec = 0;

// Function to get current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return 0;
  }
  time(&now);
  return now;
}

// Function to get timestamp in spreadsheet formula format
String getSpreadsheetFormulaTime() {
    time_t epochTime = getTime();
    epochTime += 7 * 3600; // Adjust for GMT+7
    return "=EPOCHTODATE(" + String(epochTime) + ")";
}

void setup() {
  Serial.begin(115200);
  delay(100);
  dht.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println();
  Serial.println("Connected to Wi-Fi");

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println("Time synchronized");

  GSheet.setTokenCallback([](TokenInfo info) {
    if (info.status == token_status_error) {
      Serial.printf("Token error: %s\n", GSheet.getTokenError(info).c_str());
    }
  });

  GSheet.setPrerefreshSeconds(10 * 60);
  GSheet.begin(CLIENT_EMAIL, PROJECT_ID, PRIVATE_KEY);
}

void loop() {
  if (GSheet.ready() && millis() - lastTime > timerDelay * 1000) {
    lastTime = millis();

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Failed to read from DHT sensor");
      return;
    }

    FirebaseJson response;
    FirebaseJson valueRange;

    String timeStampFormula = getSpreadsheetFormulaTime();

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

    Serial.print("Temperature: "); Serial.print(temperature); Serial.println(" °C");
    Serial.print("Humidity: "); Serial.print(humidity); Serial.println(" %");
  }
}
