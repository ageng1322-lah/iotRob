#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

//////////////////////////////////////////////////////////
// ====== BAGIAN YANG BOLEH KAMU GANTI ======
//////////////////////////////////////////////////////////

// --- WIFI ---
const char WIFI_SSID[]     = "Nggakadajaringan";      // <-- GANTI
const char WIFI_PASSWORD[] = "nggakadapassword";      // <-- GANTI

// --- SERVER ---
String HOST_NAME = "https://dronewater.hanifun.my.id"; // <-- GANTI jika domain berubah
String PATH_NAME = "/api.php";                         // <-- GANTI jika file berubah

// --- KALIBRASI PH (WAJIB DISESUAIKAN NANTI) ---
float PH_NEUTRAL_VOLTAGE = 2.368;   // <-- nanti sesuaikan dengan hasil air netral
float PH_SLOPE = 0.18;            // <-- nanti sesuaikan jika perlu

//////////////////////////////////////////////////////////

// --- PIN ---
#define TURB_PIN 34
#define PH_PIN   35

void setup() {
  Serial.begin(115200);

  analogReadResolution(12);

  // ===== WAJIB ADA UNTUK ESP32 =====
  analogSetPinAttenuation(PH_PIN, ADC_11db);
  analogSetPinAttenuation(TURB_PIN, ADC_11db);

  // WIFI CONNECT
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Target API: ");
  Serial.println(HOST_NAME + PATH_NAME);
}

void loop() {

  //////////////////////////////////////////////////////////
  // 1️⃣ BACA SENSOR DENGAN AVERAGING (BIAR STABIL)
  //////////////////////////////////////////////////////////

  long sumT = 0;
  long sumP = 0;

  for (int i = 0; i < 100; i++) {
    sumT += analogRead(TURB_PIN);
    sumP += analogRead(PH_PIN);
    delay(2);
  }

  float vT = (sumT / 100.0) * (3.3 / 4095.0);
  float vP = (sumP / 100.0) * (3.3 / 4095.0);

  //////////////////////////////////////////////////////////
  // 2️⃣ HITUNG NTU (TURBIDITY)
  //////////////////////////////////////////////////////////

  float ntu = (1.10 - vT) * 1000;
  if (ntu < 0) ntu = 0;

  //////////////////////////////////////////////////////////
  // 3️⃣ HITUNG PH (SUDAH BENAR)
  //////////////////////////////////////////////////////////

  float nilaiPH = 7 + ((PH_NEUTRAL_VOLTAGE - vP) / PH_SLOPE);

  //////////////////////////////////////////////////////////
  // 4️⃣ STATUS AIR (Sementara hanya dari NTU)
  //////////////////////////////////////////////////////////

  String statusAir = (ntu < 100) ? "BERSIH" : "KERUH";

  //////////////////////////////////////////////////////////
  // 5️⃣ DEBUG SERIAL (WAJIB UNTUK KALIBRASI)
  //////////////////////////////////////////////////////////

  Serial.print("Voltage pH: ");
  Serial.print(vP, 3);

  Serial.print(" V | pH: ");
  Serial.print(nilaiPH, 2);

  Serial.print(" | NTU: ");
  Serial.print(ntu, 0);

  Serial.print(" | Status: ");
  Serial.println(statusAir);

  //////////////////////////////////////////////////////////
  // 6️⃣ KIRIM KE SERVER (HTTPS)
  //////////////////////////////////////////////////////////

  if (WiFi.status() == WL_CONNECTED) {

    WiFiClientSecure client;
    client.setInsecure();  // agar HTTPS bisa jalan

    HTTPClient http;

    String queryString = "?kualitas_air=" + String(nilaiPH, 2)
                       + "&tahan="        + String(ntu, 0)
                       + "&udara="        + String(vT, 2)
                       + "&daya_listrik=" + String(vP, 2);

    String serverPath = HOST_NAME + PATH_NAME + queryString;

    Serial.print("Sending to: ");
    Serial.println(serverPath);

    http.begin(client, serverPath.c_str());
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

  Serial.println("------------------------------------------------");
  delay(5000);
}