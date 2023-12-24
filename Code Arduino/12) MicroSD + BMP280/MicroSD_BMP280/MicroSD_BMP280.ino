#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <FS.h>
#include <SD.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Replace with your network credentials
const char* ssid = "Tidak untuk umum";      // Isi dengan nama WiFi hotspot
const char* password = "asdlkj1234";        // Isi dengan password WiFi hotspot

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;
unsigned long epochTime;  // Deklarasi epochTime di sini

// BMP280 I2C
Adafruit_BMP280 bmp; // I2C Interface

// Variables to hold sensor readings
float temp;
float alt;
float pres;
String dataMessage;

// NTP server to request epoch time
const char* ntpServer = "pool.ntp.org";
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer);

// Function that gets current epoch time
unsigned long getTime() {
  timeClient.update();
  return timeClient.getEpochTime();
}

// Initialize WiFi
void initWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

// Init BMP280
void initBME() {
  if (!bmp.begin(0x76)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }
}


// Initialize SD card
void initSDCard() {
  if (!SD.begin(D8)) { // D8 is the default CS pin on NodeMCU (ESP8266)
    Serial.println("Card Mount Failed");
    return;
    }
  Serial.println("SD Card initialized.");
}

// Write to the SD card
void writeFile(const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = SD.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

// Append data to the SD card
void appendFile(const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = SD.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }

  // Pindahkan kursor ke akhir file
  file.seek(file.size());

  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void setup() {
  Serial.begin(115200);
  initWiFi();
  initBME();
  initSDCard();
  timeClient.begin();
  configTime(0, 0, ntpServer);

  // If the data.txt file doesn't exist
  // Create a file on the SD card and write the data labels
  File file = SD.open("/data.txt");
  if (!file) {
    Serial.println("File doesn't exist");
    Serial.println("Creating file...");
    writeFile("/data.txt", "Epoch Time, Temperature, altitude, Pressure \r\n");
  } else {
    Serial.println("File already exists");
  }
  file.close();
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    // Get epoch time
    epochTime = getTime();

    // Get sensor readings
    temp = bmp.readTemperature();
    alt = bmp.readAltitude(1019.66);
    pres = bmp.readPressure() / 100.0F;

    // Concatenate all info separated by commas
    dataMessage = String(epochTime) + "," + String(temp) + "," + String(alt) + "," + String(pres) + "\r\n";
    Serial.print("Saving data: ");
    Serial.println(dataMessage);

    // Append the data to file
    appendFile("/data.txt", dataMessage.c_str());

    lastTime = millis();
  }
}
