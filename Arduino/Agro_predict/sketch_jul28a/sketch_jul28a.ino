#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
  #include <SPI.h>
  #include <SD.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

// WiFi credentials
#define SECRET_SSID "k.L"
#define SECRET_PASS "kwakulyfstyle7017"

// Firebase project API Key and RTDB URL
#define FIREBASE_HOST "https://agro-cd5c0-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "nU9eBl464BsF1JH5WpQxKc5WPWe9WtmIk0z6rgLb"

// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

const int sensor_pin = 35;

void setup() {
  Serial.begin(115200);
  pinMode(sensor_pin, INPUT);

  Serial.println("Starting setup...");

  WiFi.begin(SECRET_SSID, SECRET_PASS);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Assign the API key (required)
  config.api_key = FIREBASE_AUTH;

  // Assign the RTDB URL (required)
  config.database_url = FIREBASE_HOST;

  // Sign up
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Sign up OK");
    signupOK = true;
  } else {
    Serial.printf("Sign up failed: %s\n", config.signer.signupError.message.c_str());
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    int sensor_data = analogRead(sensor_pin);
    Serial.print("Sensor data: ");
    Serial.print(sensor_data);
    Serial.print("\t | ");

    if (sensor_data > 3600) {
      Serial.println("No moisture, Soil is dry");
    } else if (sensor_data >= 2400 && sensor_data <= 3600) {
      Serial.println("There is some moisture, Soil is medium");
    } else if (sensor_data < 2400) {
      Serial.println("Soil is wet");
    }

    // Write the sensor data to the database path sensor/data
    if (Firebase.RTDB.setInt(&fbdo, "sensor/data", sensor_data)) {
      Serial.println("Data sent successfully");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    } else {
      Serial.println("Failed to send data");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
}
