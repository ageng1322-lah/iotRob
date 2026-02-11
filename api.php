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
    
    $kualitas_air = (float)$_GET['kualitas_air'];
    $tahan = (float)$_GET['tahan'];
    $udara = (float)$_GET['udara'];
    $daya_listrik = (float)$_GET['daya_listrik'];

    // Query untuk menyimpan data
    $sql = "INSERT INTO sensor_logs (kualitas_air, tahan, udara, daya_listrik)
            VALUES ($kualitas_air, $tahan, $udara, $daya_listrik)";

    if ($conn->query($sql) === TRUE) {
        echo "New record created successfully";
    } else {
        echo "Error: " . $sql . "<br>" . $conn->error;
    }
} else {
    echo "Parameter tidak lengkap. Harap kirimkan parameter: kualitas_air, tahan, udara, daya_listrik";
}

$conn->close();
?>
