/*
 * This ESP32 code is customized based on user request.
 * Original source: esp32io.com
 */

#include <WiFi.h>
#include <HTTPClient.h>

const char WIFI_SSID[] = "Wokwi_GUEST";         // CHANGE IT
const char WIFI_PASSWORD[] = ""; // CHANGE IT

// Ganti dengan Domain atau IP server Anda tempat api.php berada
String HOST_NAME   = "http://YOUR_DOMAIN_OR_IP"; // CHANGE IT, e.g., "http://192.168.1.100" or "http://example.com"
String PATH_NAME   = "/robotik/api.php";      // Sesuaikan path ke file api.php Anda

void setup() {
  Serial.begin(9600);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Cek koneksi WiFi sebelum mengirim request
  if ((WiFi.status() == WL_CONNECTED)) {
    HTTPClient http;

    // Simulasi data sensor (Ganti bagian ini dengan pembacaan sensor sebenarnya)
    float kualitas_air = random(0, 100) + (random(0, 100) / 100.0);
    float tahan = random(0, 1000); // Mungkin nilai resistansi atau kelembaban tanah
    float udara = random(20, 40);  // Suhu atau kualitas udara
    float daya_listrik = random(100, 220); // Tegangan atau daya
    
    // Membangun Query String
    String queryString = "?kualitas_air=" + String(kualitas_air) 
                       + "&tahan=" + String(tahan) 
                       + "&udara=" + String(udara) 
                       + "&daya_listrik=" + String(daya_listrik);

    // URL Lengkap
    String serverPath = HOST_NAME + PATH_NAME + queryString;
    
    Serial.print("Requesting URL: ");
    Serial.println(serverPath);

    http.begin(serverPath.c_str());
    
    // Kirim HTTP GET request
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println("Server Response: " + payload);
      } else {
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
  
  // Tunggu 5 detik sebelum mengirim data lagi
  delay(5000);
}
