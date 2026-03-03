#include <WiFi.h>
#include <HTTPClient.h>

// --- KONFIGURASI WIFI ---
const char WIFI_SSID[]     = "Nggakadajaringan";
const char WIFI_PASSWORD[] = "nggakadapassword";

// --- KONFIGURASI SERVER ---
// ✅ PERBAIKAN: Hapus trailing slash "/" di akhir HOST_NAME
// Salah  : "http://dronewater.hanifun.my.id/"  → menghasilkan //api.php (double slash!)
// Benar  : "http://dronewater.hanifun.my.id"   → menghasilkan /api.php  ✓
String HOST_NAME = "http://dronewater.hanifun.my.id"; // ← TIDAK ADA / di akhir
String PATH_NAME = "/api.php";

// --- KONFIGURASI PIN ---
#define TURB_PIN 34
#define PH_PIN   35

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Tampilkan URL yang akan dipakai — pastikan TIDAK ada double slash
  Serial.print("Target API : ");
  Serial.println(HOST_NAME + PATH_NAME);
}

void loop() {
  // 1. PEMBACAAN SENSOR (AVERAGING)
  long sumT = 0;
  long sumP = 0;
  for (int i = 0; i < 100; i++) {
    sumT += analogRead(TURB_PIN);
    sumP += analogRead(PH_PIN);
    delay(1);
  }

  float vT = (sumT / 100.0) * (3.3 / 4095.0);
  float vP = (sumP / 100.0) * (3.3 / 4095.0);

  // 2. KALIBRASI & PERHITUNGAN
  float ntu     = (1.65 - vT) * 1000;
  if (ntu < 0) ntu = 0;

  float nilaiPH = 7.0 + ((1.65 - vP) * 3.5) + 3.8;
  nilaiPH = constrain(nilaiPH, 0.0, 14.0);

  String statusAir = (ntu < 100 && nilaiPH >= 6.0 && nilaiPH <= 8.5) ? "BERSIH" : "KERUH";

  // 3. TAMPILAN SERIAL MONITOR
  Serial.print("pH: ");     Serial.print(nilaiPH, 1);
  Serial.print(" | NTU: "); Serial.print(ntu, 0);
  Serial.print(" | Status: "); Serial.println(statusAir);

  // 4. KIRIM DATA KE DASHBOARD
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String queryString = "?kualitas_air=" + String(nilaiPH, 1)
                       + "&tahan="        + String(ntu, 0)
                       + "&udara="        + String(vT, 2)
                       + "&daya_listrik=" + String(vP, 2);

    // ✅ URL yang benar: http://dronewater.hanifun.my.id/api.php?...
    String serverPath = HOST_NAME + PATH_NAME + queryString;

    Serial.print("Sending to: ");
    Serial.println(serverPath);

    http.begin(serverPath.c_str());
    http.setTimeout(10000);
    int httpCode = http.GET();

    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println("Response: " + payload);
      } else {
        Serial.printf("HTTP Code: %d\n", httpCode);
      }
    } else {
      Serial.printf("Error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();

  } else {
    Serial.println("WiFi Disconnected!");
    WiFi.reconnect();
  }

  Serial.println("-----------------------------------------------------------");
  delay(5000);
}
