#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <BH1750.h>

// Inisialisasi LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); // Alamat modul LCD 16x2 I2C (0x27), jumlah kolom (16), jumlah baris (2)

// Pin dan variabel untuk pembaca pH
const int ph_Pin = A0;
float Po = 0;
float PH_step;
double TeganganPh;
float PH4 = 3.4;
float PH7 = 2.5;

// Pin dan variabel untuk pembaca TDS
const int sensorPin_TDS = A1;  // Pin analog untuk membaca TDS
float voltage_TDS;
float TDSValue;

// Pin dan variabel untuk pembaca level air dan relay
const int sensorPin_waterLevel = 2;    // Pin untuk sensor water level
const int relayPin = 7;                // Pin untuk mengontrol relay

// Pin dan variabel untuk pembaca kelembapan tanah
const int moistureSensorPin = A2;  // Pin tempat Anda menghubungkan output sensor ke pin A2 Arduino

// Inisialisasi sensor intensitas cahaya
BH1750 lightMeter;

void setup() {
  Serial.begin(9600);  // Inisialisasi komunikasi serial
  lcd.init();           // Inisialisasi LCD
  lcd.backlight();      // Nyalakan backlight LCD
  lightMeter.begin();   // Inisialisasi sensor cahaya

  pinMode(ph_Pin, INPUT);           // Pin untuk pembaca pH sebagai input
  pinMode(sensorPin_TDS, INPUT);    // Pin untuk pembaca TDS sebagai input
  pinMode(sensorPin_waterLevel, INPUT); // Pin untuk pembaca level air sebagai input
  pinMode(relayPin, OUTPUT);        // Pin untuk mengontrol relay sebagai output
}

void loop() {
  // Pembaca pH
  int nilai_analog_PH = analogRead(ph_Pin);
  TeganganPh = 5.0 / 1024.0 * nilai_analog_PH;
  PH_step = (PH4 - PH7) / 3;
  Po = 7.00 + ((PH7 - TeganganPh) / PH_step);

  // Menampilkan nilai pH pada Serial Monitor
  Serial.print("Nilai pH Cairan: ");
  Serial.println(Po, 2);

  // Menentukan kondisi "buruk" atau "baik" berdasarkan nilai pH
  String kondisi_PH = (Po < 6 || Po > 8) ? "Buruk" : "Baik";

  // Menampilkan nilai pH dan kondisi pada LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Nilai pH: ");
  lcd.print(Po, 2);
  lcd.setCursor(0, 1);
  lcd.print("Kondisi: ");
  lcd.print(kondisi_PH);

  delay(3000);  // Delay 3 detik sebelum pembacaan ulang

  // Pembaca TDS
  int sensorValue_TDS = analogRead(sensorPin_TDS);
  voltage_TDS = sensorValue_TDS * (5.0 / 1023.0);
  TDSValue = (133.42 * voltage_TDS * voltage_TDS * voltage_TDS - 255.86 * voltage_TDS * voltage_TDS + 857.39 * voltage_TDS) * 0.5;

  // Menampilkan nilai TDS pada Serial Monitor
  Serial.print("TDS: ");
  Serial.print(TDSValue);
  Serial.println(" ppm");

  // Menentukan kualitas air berdasarkan nilai TDS
  String kualitas_air = "";
  if (TDSValue < 300) {
    kualitas_air = "Sangat Baik";
  } else if (TDSValue < 600) {
    kualitas_air = "Baik";
  } else if (TDSValue < 900) {
    kualitas_air = "Memuaskan";
  } else {
    kualitas_air = "Buruk";
  }

  // Menampilkan nilai TDS dan kualitas air pada LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TDS: ");
  lcd.print(TDSValue);
  lcd.print(" ppm");
  lcd.setCursor(0, 1);
  lcd.print("Kualitas Air: ");
  lcd.print(kualitas_air);

  delay(3000);  // Delay 3 detik sebelum pembacaan ulang

  // Pembaca level air dan kontrol relay
  int waterLevel = digitalRead(sensorPin_waterLevel);

  // Jika sensor mendeteksi air kosong (misalnya output LOW)
  if (waterLevel == LOW) {
    digitalWrite(relayPin, HIGH);  // Nyalakan relay untuk memompa air
    delay(1000);  // Tahan relay selama 1 detik (opsional)
    digitalWrite(relayPin, LOW);   // Matikan relay
  }

  delay(100);  // Delay untuk stabilitas program

  // Pembaca kelembapan tanah
  int sensorValue_moisture = analogRead(moistureSensorPin);
  float moisturePercentage = map(sensorValue_moisture, 0, 1023, 0, 100);

  // Menampilkan nilai kelembaban tanah pada Serial Monitor
  Serial.print("Kelembaban Tanah: ");
  Serial.print(moisturePercentage);
  Serial.println("%");

  // Menentukan keterangan index kelembaban tanah
  String kondisi_kelembaban = "";
  if (moisturePercentage < 20.0) {
    kondisi_kelembaban = "Sangat Kering";
  } else if (moisturePercentage < 30.0) {
    kondisi_kelembaban = "Kering";
  } else if (moisturePercentage < 50.0) {
    kondisi_kelembaban = "Cukup Baik";
  } else if (moisturePercentage < 70.0) {
    kondisi_kelembaban = "Basah";
  } else {
    kondisi_kelembaban = "Sangat Basah";
  }

  // Menampilkan nilai kelembaban tanah dan keterangan index pada LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Kelembaban: ");
  lcd.print(moisturePercentage);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print(kondisi_kelembaban);

  delay(3000);  // Delay 3 detik sebelum pembacaan ulang

  // Pembaca intensitas cahaya
  uint16_t lux = lightMeter.readLightLevel();

  // Menampilkan nilai intensitas cahaya pada Serial Monitor
  Serial.print("Lux: ");
  Serial.println(lux);

  // Menentukan keterangan index intensitas cahaya
  String kondisi_cahaya = "";
  if (lux < 2000) {
    kondisi_cahaya = "Intens. Rendah";
  } else if (lux < 10000) {
    kondisi_cahaya = "Intens. Sedang";
  } else {
    kondisi_cahaya = "Intens. Tinggi";
  }

  // Menampilkan nilai intensitas cahaya dan keterangan index pada LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Lux: ");
  lcd.print(lux);
  lcd.setCursor(0, 1);
  lcd.print(kondisi_cahaya);

  delay(3000);  // Delay 3 detik sebelum pembacaan ulang
}
