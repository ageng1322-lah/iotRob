/*
 * ============================================================
 * DRONE UNDERWATER — ESP32 Sensor Data Sender
 * Mengirim data sensor ke server hosting via WiFi (HTTP GET)
 * ============================================================
 * Target API : http://DOMAIN_ANDA/robotik/api.php
 * Interval   : Setiap 5 detik
 * ============================================================
 */

#include <WiFi.h>
#include <HTTPClient.h>

// ============================================================
// KONFIGURASI WIFI — Ganti sesuai jaringan WiFi Anda
// ============================================================
const char WIFI_SSID[]     = "NAMA_WIFI_ANDA";     // <<< GANTI INI
const char WIFI_PASSWORD[] = "PASSWORD_WIFI_ANDA"; // <<< GANTI INI

// ============================================================
// KONFIGURASI SERVER HOSTING
// Ganti dengan domain hosting Anda, contoh:
//   "http://hanifunm.000webhostapp.com"  atau
//   "http://192.168.1.100" (jika menggunakan IP lokal)
// ============================================================
const String HOST_NAME = "https://DOMAIN_HOSTING_ANDA"; // <<< GANTI INI
const String PATH_NAME = "/robotik/api.php";             // Path ke file api.php

// ============================================================
// PIN SENSOR (sesuaikan dengan wiring drone Anda)
// ============================================================
// Contoh pin jika menggunakan sensor analog:
// const int PIN_TURBIDITY  = 34; // Sensor kekeruhan air
// const int PIN_DEPTH      = 35; // Sensor tekanan/kedalaman
// const int PIN_TEMP       = 32; // Sensor suhu
// const int PIN_VOLTAGE    = 33; // Sensor daya baterai

// Interval pengiriman data (milidetik)
const unsigned long SEND_INTERVAL = 5000; // 5 detik
unsigned long lastSendTime = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("\n=== DRONE UNDERWATER SENSOR SYSTEM ===");

    // Sambungkan ke WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Menghubungkan ke WiFi: ");
    Serial.print(WIFI_SSID);

    int attempt = 0;
    while (WiFi.status() != WL_CONNECTED && attempt < 30) {
        delay(500);
        Serial.print(".");
        attempt++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n✓ WiFi Terhubung!");
        Serial.print("  IP Address : ");
        Serial.println(WiFi.localIP());
        Serial.print("  Signal     : ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");
    } else {
        Serial.println("\n✗ Gagal terhubung ke WiFi. Cek SSID/Password.");
    }
}

void loop() {
    unsigned long currentTime = millis();

    // Kirim data setiap SEND_INTERVAL milidetik
    if (currentTime - lastSendTime >= SEND_INTERVAL) {
        lastSendTime = currentTime;

        if (WiFi.status() == WL_CONNECTED) {
            sendSensorData();
        } else {
            Serial.println("⚠ WiFi terputus, mencoba reconnect...");
            WiFi.reconnect();
            delay(3000);
        }
    }
}

void sendSensorData() {
    // ============================================================
    // BACA DATA SENSOR
    // Ganti bagian simulasi ini dengan pembacaan sensor sebenarnya
    // ============================================================

    // [SIMULASI] — Ganti dengan analogRead(PIN_SENSOR) saat deploy
    float kualitas_air = random(60, 100) + (random(0, 99) / 100.0); // WQI: 0-100
    float tahan        = random(10, 900);                             // Resistansi/Kedalaman
    float udara        = random(20, 40) + (random(0, 99) / 100.0);   // Suhu / AQI
    float daya_listrik = random(100, 220) + (random(0, 99) / 100.0); // Tegangan baterai (V/W)

    /*
     * Contoh pembacaan sensor NYATA (uncomment jika sudah pasang sensor):
     *
     * int rawTurbidity  = analogRead(PIN_TURBIDITY);
     * float kualitas_air = map(rawTurbidity, 0, 4095, 0, 100);
     *
     * int rawDepth  = analogRead(PIN_DEPTH);
     * float tahan   = rawDepth * (3.3 / 4095.0) * 100.0; // konversi tegangan
     *
     * float udara        = dht.readTemperature();
     * float daya_listrik = ina.readBusVoltage();
     */

    // ============================================================
    // KIRIM DATA KE SERVER VIA HTTP GET
    // ============================================================
    HTTPClient http;

    String queryString = "?kualitas_air=" + String(kualitas_air, 2)
                       + "&tahan="        + String(tahan, 2)
                       + "&udara="        + String(udara, 2)
                       + "&daya_listrik=" + String(daya_listrik, 2);

    String serverPath = HOST_NAME + PATH_NAME + queryString;

    Serial.println("\n--- Mengirim Data Sensor ---");
    Serial.println("URL: " + serverPath);

    http.begin(serverPath.c_str());
    http.setTimeout(10000); // Timeout 10 detik

    int httpCode = http.GET();

    if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            Serial.println("✓ Server Response: " + payload);
        } else {
            Serial.printf("⚠ HTTP Code: %d\n", httpCode);
        }
    } else {
        Serial.printf("✗ HTTP Error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();

    // Log data yang dikirim ke Serial Monitor
    Serial.println("--- Data Terkirim ---");
    Serial.printf("  Kualitas Air  : %.2f WQI\n", kualitas_air);
    Serial.printf("  Tahan/Kedalaman: %.2f\n", tahan);
    Serial.printf("  Udara/Suhu    : %.2f °C\n", udara);
    Serial.printf("  Daya Listrik  : %.2f W\n", daya_listrik);
}
