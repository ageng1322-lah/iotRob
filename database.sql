-- ============================================================
-- DRONE UNDERWATER — Database Schema
-- Jalankan script ini di phpMyAdmin hosting Anda
-- Database: hanifunm_dronewater
-- ============================================================

-- Jangan buat database baru jika sudah ada di hosting!
-- USE hanifunm_dronewater;

-- Hapus tabel lama jika perlu reset (hati-hati: data akan hilang!)
-- DROP TABLE IF EXISTS sensor_logs;

-- Buat tabel sensor_logs
CREATE TABLE IF NOT EXISTS sensor_logs (
    id           INT AUTO_INCREMENT PRIMARY KEY,
    timestamp    TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    kualitas_air FLOAT NOT NULL COMMENT 'Water Quality Index (WQI), range 0-100',
    tahan        FLOAT NOT NULL COMMENT 'Kedalaman / Resistansi / Soil Index (cm / ohm)',
    udara        FLOAT NOT NULL COMMENT 'Suhu / Kualitas Udara (AQI / Celcius)',
    daya_listrik FLOAT NOT NULL COMMENT 'Daya / Tegangan Baterai (Watt / Volt)',

    INDEX idx_timestamp (timestamp)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci
  COMMENT='Log data sensor drone underwater dari ESP32';

-- ============================================================
-- DATA SAMPEL (opsional, untuk testing dashboard)
-- ============================================================
INSERT INTO sensor_logs (kualitas_air, tahan, udara, daya_listrik) VALUES
(85.20,  120.50, 28.40, 180.00),
(82.10,  145.30, 29.10, 175.50),
(88.70,   98.20, 27.80, 182.30),
(79.40,  200.10, 30.50, 170.00),
(91.50,   75.60, 26.90, 190.10),
(83.30,  165.80, 28.70, 177.40),
(87.20,  110.40, 27.30, 183.60),
(80.80,  188.70, 29.80, 172.10),
(94.10,   60.20, 26.50, 195.00),
(76.50,  230.90, 31.20, 165.80);

-- ============================================================
-- VERIFIKASI
-- ============================================================
-- SELECT * FROM sensor_logs ORDER BY id DESC LIMIT 10;
