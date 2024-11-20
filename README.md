# Praktikum-IoT JOB 3 - Pengkondisian dan Pengolahan Data
## About
Job ini berfokus pada pengolahan data dengan platform Blynk, termasuk pengaturan notifikasi push pada aplikasi Blynk saat kondisi tertentu tercapai.
## Getting Started
### Prerequisites
- Clear JOB 2.
- Blynk installation

### Installation

1. Siapkan komponen-komponen berikut
   - ESP32 atau ESP8266
   - sensor suhu DHT11 atau DHT22
   - Sensor Ultrasonic
   - Project board
   - kabel Jumper secukupnya
   - Laptop
   - Kabel data 
3. Create wiring diagram like a picture below for ESP32:

   ![wiring-diagram-image](img/wiring-esp32.png)

   And like a picture below for ESP8266

   ![wiring-diagram-image](img/wiring-esp8266.png)
   
4. Program menggunakan program (jika anda menggunakan ESP32)->  [ESP32 program](esp32-temp-distance/esp32-temp-distance.ino)
5. Program menggunakan program (jika anda menggunakan ESP8266)->  [ESP8266 program](esp8266-temp-distance/esp8266-temp-distance.ino)
6. Pada program baris ke `14` dan `15` yang berisikan program ssid dan passw ssid, silahkan sesuaikan dengan nama ssid yang akan anda gunakan untuk koneksi internet
    ```plaintext
    const char* ssid     = "your-ssid"; //your wifi ssid
    const char* password = "ssid-passw"; //your wifi password
    ```
