<?php
// Konfigurasi Database
$servername = "localhost";
$username = "lahsamyi_iotrobot";       // Ganti dengan username database Anda
$password = "RHdV45zpw2DsZRzEvz3D";           // Ganti dengan password database Anda
$dbname = "lahsamyi_iotrobot";    // Nama database

// Membuat koneksi ke database
$conn = new mysqli($servername, $username, $password, $dbname);

// Cek koneksi
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Menerima data dari method GET (sesuai code Arduino yang menggunakan GET)
// Contoh URL: http://domain.com/api.php?kualitas_air=10&tahan=20&udara=30&daya_listrik=40

if (isset($_GET['kualitas_air']) && isset($_GET['tahan']) && isset($_GET['udara']) && isset($_GET['daya_listrik'])) {

    $kualitas_air = (float) $_GET['kualitas_air'];
    $tahan = (float) $_GET['tahan'];
    $udara = (float) $_GET['udara'];
    $daya_listrik = (float) $_GET['daya_listrik'];

    // Query untuk menyimpan data
    $sql = "INSERT INTO sensor_logs (kualitas_air, tahan, udara, daya_listrik)
            VALUES ($kualitas_air, $tahan, $udara, $daya_listrik)";

    if ($conn->query($sql) === TRUE) {
        echo "New record created successfully";
    } else {
        echo "Error: " . $sql . "<br>" . $conn->error;
    }
} else {
    // Mode Read
    header('Content-Type: application/json');

    if (isset($_GET['mode']) && $_GET['mode'] == 'history') {
        // Ambil 20 data terakhir untuk grafik, urutkan ASC agar timeline benar (kiri ke kanan)
        // Subquery digunakan untuk mengambil 20 terakhir, lalu di-order ASC
        $sql = "SELECT * FROM (
                    SELECT * FROM sensor_logs ORDER BY id DESC LIMIT 20
                ) AS sub ORDER BY id ASC";
    } else {
        // Ambil 1 data terakhir untuk real-time stats
        $sql = "SELECT * FROM sensor_logs ORDER BY id DESC LIMIT 1";
    }

    $result = $conn->query($sql);

    $data = array();
    if ($result->num_rows > 0) {
        if (isset($_GET['mode']) && $_GET['mode'] == 'history') {
            while ($row = $result->fetch_assoc()) {
                $data[] = $row;
            }
            echo json_encode($data);
        } else {
            $row = $result->fetch_assoc();
            echo json_encode($row);
        }
    } else {
        echo json_encode(["error" => "No data found"]);
    }
}

$conn->close();
?>