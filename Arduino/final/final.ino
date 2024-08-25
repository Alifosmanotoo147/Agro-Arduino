#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <HTTPClient.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <DHT.h>

// WiFi credentials
#define WIFI_SSID "Wilsonwils❤️"
#define WIFI_PASSWORD "Xzibit5+"

// Ngrok server URL
#define NGROK_URL "https://0d6b-154-161-5-86.ngrok-free.app/api/vehicle/update"

// Sensor pins
#define On_Board_LED 2
#define SENSOR_PIN 35
#define DHT_PIN 1

// DHT sensor type
#define DHT_TYPE DHT11

// RS485 control pins
#define RE 15
#define DE 5

// Modbus commands for NPK sensors
const byte nitro[] = {0x01,0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c};
const byte phos[] = {0x01,0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc};
const byte pota[] = {0x01,0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};

byte values[11];
SoftwareSerial mod(2,3);
DHT dht(DHT_PIN, DHT_TYPE);

// Timing variables
unsigned long sendDataPrevMillis = 0;
const long sendDataIntervalMillis = 10000; // 10 seconds

void setup() {
  Serial.begin(115200);
  mod.begin(9600);
  
  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);
  pinMode(On_Board_LED, OUTPUT);
  pinMode(SENSOR_PIN, INPUT);
  
  dht.begin();
  
  // Connecting to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to WiFi...");
  
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    Serial.print(".");
    digitalWrite(On_Board_LED, HIGH);
    delay(250);
    digitalWrite(On_Board_LED, LOW);
    delay(250);
  }
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nFailed to connect to WiFi. Restarting...");
    ESP.restart();
  }
  
  digitalWrite(On_Board_LED, LOW);
  Serial.println("\nSuccessfully connected to WiFi");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED && (millis() - sendDataPrevMillis > sendDataIntervalMillis || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    
    // Read NPK values
    byte nitrogenValue = nitrogen();
    byte phosphorousValue = phosphorous();
    byte potassiumValue = potassium();
    
    // Read soil moisture
    int soilMoisture = analogRead(SENSOR_PIN);
    
    // Read temperature and humidity
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    
    // Determine soil status
    String soilStatus;
    if (soilMoisture > 3600) {
      soilStatus = "No moisture, Soil is dry";
    } else if (soilMoisture >= 2400 && soilMoisture <= 3600) {
      soilStatus = "There is some moisture, Soil is medium";
    } else {
      soilStatus = "Soil is wet";
    }
    
    // Prepare JSON payload
    String payload = "{\"nitrogen\": " + String(nitrogenValue) + 
                     ", \"phosphorous\": " + String(phosphorousValue) +
                     ", \"potassium\": " + String(potassiumValue) +
                     ", \"soil_moisture\": " + String(soilMoisture) +
                     ", \"temperature\": " + String(temperature) +
                     ", \"humidity\": " + String(humidity) +
                     ", \"soil_status\": \"" + soilStatus + "\"}";
    
    // Send data to Ngrok server
    if (sendDataToNgrok(payload)) {
      Serial.println("Data sent successfully");
    } else {
      Serial.println("Failed to send data");
    }
  }
  delay(100);
}

byte nitrogen() {
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);
  if (mod.write(nitro, sizeof(nitro)) == 8) {
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);
    for (byte i = 0; i < 7; i++) {
      values[i] = mod.read();
      Serial.print(values[i], HEX);
    }
    Serial.println();
  }
  return values[4];
}

byte phosphorous() {
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);
  if (mod.write(phos, sizeof(phos)) == 8) {
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);
    for (byte i = 0; i < 7; i++) {
      values[i] = mod.read();
      Serial.print(values[i], HEX);
    }
    Serial.println();
  }
  return values[4];
}

byte potassium() {
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);
  if (mod.write(pota, sizeof(pota)) == 8) {
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);
    for (byte i = 0; i < 7; i++) {
      values[i] = mod.read();
      Serial.print(values[i], HEX);
    }
    Serial.println();
  }
  return values[4];
}

bool sendDataToNgrok(String payload) {
  HTTPClient http;
  http.begin(NGROK_URL);
  http.addHeader("Content-Type", "application/json");
  
  int httpResponseCode = http.POST(payload);
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("HTTP Response code: " + String(httpResponseCode));
    Serial.println("Response: " + response);
    http.end();
    return true;
  } else {
    Serial.println("Error on sending POST: " + String(httpResponseCode));
    http.end();
    return false;
  }
}
