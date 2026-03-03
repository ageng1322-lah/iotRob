<?php
// ============================================================
// CORS HEADERS - Wajib agar dashboard bisa mengakses API
// dari domain apapun (termasuk saat hosting terpisah)
// ============================================================
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: GET, POST, OPTIONS');
header('Access-Control-Allow-Headers: Content-Type');
header('Content-Type: application/json');

// Jika request preflight OPTIONS, langsung return
if ($_SERVER['REQUEST_METHOD'] === 'OPTIONS') {
    http_response_code(200);
    exit();
}

// ============================================================
// Konfigurasi Database (sesuai dengan hosting Anda)
// ============================================================
$servername = "localhost";
$username = "hanifunm_drone_water_2";  // Username database hosting
$password = "HMxh3WneUM2CfsrrXV4Y"; // Password database hosting
$dbname = "hanifunm_drone_water_2";  // Nama database di hosting

// Membuat koneksi ke database
$conn = new mysqli($servername, $username, $password, $dbname);

// Cek koneksi
if ($conn->connect_error) {
    echo json_encode([
        "status" => "error",
        "message" => "Connection failed: " . $conn->connect_error
    ]);
    exit();
}

// ============================================================
// MODE: WRITE — Menerima data sensor dari ESP32 via WiFi
// Contoh URL: http://domain.com/robotik/api.php?kualitas_air=85&tahan=20&udara=30&daya_listrik=40
// ============================================================
if (
    isset($_GET['kualitas_air']) &&
    isset($_GET['tahan']) &&
    isset($_GET['udara']) &&
    isset($_GET['daya_listrik'])
) {
    $kualitas_air = (float) $_GET['kualitas_air'];
    $tahan = (float) $_GET['tahan'];
    $udara = (float) $_GET['udara'];
    $daya_listrik = (float) $_GET['daya_listrik'];

    // Gunakan prepared statement untuk keamanan
    $stmt = $conn->prepare(
        "INSERT INTO sensor_logs (kualitas_air, tahan, udara, daya_listrik) VALUES (?, ?, ?, ?)"
    );
    $stmt->bind_param("dddd", $kualitas_air, $tahan, $udara, $daya_listrik);

    if ($stmt->execute()) {
        echo json_encode([
            "status" => "success",
            "message" => "Data sensor berhasil disimpan",
            "data" => [
                "kualitas_air" => $kualitas_air,
                "tahan" => $tahan,
                "udara" => $udara,
                "daya_listrik" => $daya_listrik
            ]
        ]);
    } else {
        echo json_encode([
            "status" => "error",
            "message" => "Gagal menyimpan data: " . $stmt->error
        ]);
    }
    $stmt->close();

    // ============================================================
// MODE: READ — Membaca data untuk dashboard
// ============================================================
} else {
    if (isset($_GET['mode']) && $_GET['mode'] === 'history') {
        // Ambil 20 data terakhir untuk grafik, urutkan ASC agar timeline benar
        $sql = "SELECT * FROM (
                    SELECT id, kualitas_air, tahan, udara, daya_listrik, timestamp
                    FROM sensor_logs
                    ORDER BY id DESC LIMIT 20
                ) AS sub ORDER BY id ASC";
    } else {
        // Ambil 1 data terakhir untuk real-time stats
        $sql = "SELECT id, kualitas_air, tahan, udara, daya_listrik, timestamp
                FROM sensor_logs
                ORDER BY id DESC LIMIT 1";
    }

    $result = $conn->query($sql);
    $data = [];

    if ($result && $result->num_rows > 0) {
        if (isset($_GET['mode']) && $_GET['mode'] === 'history') {
            while ($row = $result->fetch_assoc()) {
                $data[] = $row;
            }
            echo json_encode($data);
        } else {
            $row = $result->fetch_assoc();
            echo json_encode($row);
        }
    } else {
        echo json_encode(["error" => "Belum ada data sensor"]);
    }
}

$conn->close();
?>