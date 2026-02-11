-- Membuat database jika belum ada
CREATE DATABASE IF NOT EXISTS sensor_db;
USE sensor_db;

-- Membuat tabel sensor_logs sesuai permintaan user
-- Kolom: timestamp, kualitas_air, tahan, udara, daya_listrik
CREATE TABLE IF NOT EXISTS sensor_logs (
    id INT AUTO_INCREMENT PRIMARY KEY,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP, -- Otomatis terisi waktu saat data masuk
    kualitas_air FLOAT NOT NULL,
    tahan FLOAT NOT NULL, -- Sesuai request 'tahan' (mungkin maksudnya 'tanah' / soil moisture atau resistansi)
    udara FLOAT NOT NULL,
    daya_listrik FLOAT NOT NULL
);
