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
   
4. Copy isi dari file  [ESP32 program](program-subs-mqtt/program-subs-mqtt.ino)
5. Paste kan pada project wokwi anda
6. Pada program baris ke `12.` yang berisikan program seperti berikut, silahkan ubah `PRAKTIKIOT/ARIFF` menjadi nama topic untuk koneksi MQTT anda
    ```plaintext
    const char* topic = "PRAKTIKIOT/ARIFF";
    ```
7. Buka HiveMQ websocket client, lalu masukan nama topic yang sama seperti pada program ESP pada tab subscribtions
8. Pada tab publish di HiveMQ websocket client silahkan masukan nama topic yang sama seperti pada program ESP
9. Saat MQTT connected pada ESP, masukan angka 1 atau 0 lalu tekan publish, amati apakah data 1 atau 0 ini masuk ke ESP dan lampu pada ESP hidup mati 
