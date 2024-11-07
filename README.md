# Praktikum-IoT JOB 1 - Dasar IoT dan Koneksi Jaringan 
## About
Simulasi publish data dari ESP32 pada Wokwi ke broker public MQTT gratis HiveMQ
## Getting Started
### Prerequisites
- Wokwi simulation at https://wokwi.com/.
- HiveMQ websocket client at https://www.hivemq.com/demos/websocket-client/.

### Installation

1. Create wokwi project using microcontroller ESP32
2. Create wiring diagram like a picture below:

   ![wiring-diagram-image](img/wiring.png)
   
4. Copy isi dari file  [ESP32 program](program/program.ino)
5. Paste kan pada project wokwi anda
6. Pada program baris ke `14.` yang berisikan program seperti berikut, silahkan ubah `PRAKTIKIOT/ARIFF` menjadi nama topic untuk koneksi MQTT anda
    ```plaintext
    const char* topic = "PRAKTIKIOT/ARIFF";
    ```
