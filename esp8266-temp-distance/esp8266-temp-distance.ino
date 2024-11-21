#define BLYNK_TEMPLATE_ID "..."
#define BLYNK_TEMPLATE_NAME "..."
#define BLYNK_AUTH_TOKEN "..."

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <BlynkSimpleEsp8266.h>

#define BLYNK_PRINT Serial
#define DHT_PIN D2

const char* ssid     = "realme C17";
const char* password = "12345678";

char blynkAuthToken[34];
DHT dht(DHT_PIN, DHT11);
float temp = 0;
float humi = 0;

BlynkTimer timer;

void sendToBlynk(float temp, float humi) {
  Blynk.virtualWrite(V1, temp);
  Blynk.virtualWrite(V0, humi);
}

void setup_wifi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  strcpy(blynkAuthToken, BLYNK_AUTH_TOKEN);
  setup_wifi();
  dht.begin();
  Blynk.config(blynkAuthToken);
  Blynk.connect();

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

    static bool tempAlarmSent = false;
    static bool humiAlarmSent = false;

    if (temp > 35.0 && !tempAlarmSent) {
      Blynk.logEvent("temperature_alarm", "Suhu terlalu panas");
      tempAlarmSent = true;
    } else if (temp <= 35.0) {
      tempAlarmSent = false;
    }

    if (humi > 80.0 && !humiAlarmSent) {
      Blynk.logEvent("humidity_alarm", "Ruangan Terlalu lembab, buka jendela!");
      humiAlarmSent = true;
    } else if (humi <= 80.0) {
      humiAlarmSent = false;
    }

    sendToBlynk(temp, humi);
  });
}

void loop() {
  Blynk.run();
  timer.run();
}
