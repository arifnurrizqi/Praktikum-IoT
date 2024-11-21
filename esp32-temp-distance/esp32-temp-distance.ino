// Blynk Configuration
#define BLYNK_TEMPLATE_ID  "template_id_here"
#define BLYNK_TEMPLATE_NAME "template_name_here"
#define BLYNK_AUTH_TOKEN    "auth_token_here"

#include <Arduino.h>
#include <ArduinoJson.h>          // https://github.com/bblanchon/ArduinoJson
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <BlynkSimpleEsp32.h>      // Library for blynk IoT

#define DHT_PIN 21 // Pin GPIO untuk sensor 

const char* ssid     = ""; //your wifi ssid
const char* password = ""; //your wifi password

char blynkTemplateId[40];
char blynkTemplateName[40];
char blynkAuthToken[34];

WiFiClient espClient;
DHT dht(DHT_PIN, DHT11);
float temp = 0;
float humi = 0;

// Blynk setup
BlynkTimer timer;

void sendToBlynk(float temp, float humi) {
  Blynk.virtualWrite(V0, temp);          // Send CO2 data to Blynk
  Blynk.virtualWrite(V1, humi);           // Send CO data to Blynk
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  dht.begin();

  Blynk.config(blynkAuthToken);
  Blynk.connect();

  // Setup a function to be called every second
  timer.setInterval(1000L, []() {

    temp = dht.readTemperature();
    humi = dht.readHumidity();

    Serial.print("Temperature: ");
    Serial.println(temp);
    Serial.print("Humidity: ");
    Serial.println(humi);
    

    Blynk.logEvent("gas_safe_alarm", "AMAN! Kadar CO atau CO2 RENDAH");

    if (temp > 35.0 ) {
      Serial.println("Status: Panas");

      Blynk.logEvent("temperature_alarm", "Suhu terlalu panas");   
    }
    
    if ( humi > 80.0) {
      Serial.println("Status: Lembab - Ruangan Terlalu Lembab");

      Blynk.logEvent("humidity_alarm", "Ruangan Terlalu lembab, buka jendela!");
    }
    
    // Send data to Blynk
    sendToBlynk(temp, humi);
    delay(500);
  });
}


void loop() {
  Blynk.run();
  timer.run();
}
