#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <HTTPClient.h>

// Insert your network credentials.
#define WIFI_SSID "Galaxy A02s1685"
#define WIFI_PASSWORD "gikk3536"

// Insert Ngrok server URL
#define NGROK_URL "https://139e-154-161-20-3.ngrok-free.app/predict/prediction-input" // endpoint for server

// Define the Digital Pin of the "On Board LED".
#define ON_BOARD_LED 2

// Millis variable to send/store data to Ngrok server.
unsigned long sendDataPrevMillis = 0;
const long sendDataIntervalMillis = 10000; // Sends/stores data to Ngrok server every 10 seconds.

// Variables to store simulated sensor values
float N = 63.0;
float P = 40.0;
float K = 20.0;
float temperature = 27.0;
float humidity = 60.0;

// Function to generate random values around a central value with a specific range
float generateRandomValue(float centralValue, float range) {
  return centralValue + ((float)random(-range * 100, range * 100) / 100.0);
}

void connectToWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("---------------Connection");
  Serial.print("Connecting to : ");
  Serial.println(WIFI_SSID);
  
  unsigned long startAttemptTime = millis();

  // Attempt to connect for 10 seconds
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    Serial.print(".");
    digitalWrite(ON_BOARD_LED, HIGH);
    delay(250);
    digitalWrite(ON_BOARD_LED, LOW);
    delay(250);
  }

  // Check if the ESP32 is connected to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println();
    Serial.println("Failed to connect to WiFi. Restarting...");
    ESP.restart();
  }

  digitalWrite(ON_BOARD_LED, LOW);
  Serial.println();
  Serial.print("Successfully connected to : ");
  Serial.println(WIFI_SSID);
  Serial.println("---------------");
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  pinMode(ON_BOARD_LED, OUTPUT);

  connectToWiFi();

  // Seed the random number generator
  randomSeed(analogRead(0));
}

void loop() {
  if (WiFi.status() == WL_CONNECTED && (millis() - sendDataPrevMillis > sendDataIntervalMillis || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();

    // Simulate values
    N = generateRandomValue(63.0, 5.0); // N will hover around 63.0 ± 5.0
    P = generateRandomValue(40.0, 3.0); // P will hover around 40.0 ± 3.0
    K = generateRandomValue(20.0, 2.0); // K will hover around 20.0 ± 2.0
    temperature = generateRandomValue(27.0, 2.0); // Temperature will hover around 27.0 ± 2.0
    humidity = generateRandomValue(60.0, 10.0); // Humidity will hover around 60.0 ± 10.0
    
    // Print the simulated values to the Serial Monitor
    Serial.print(" N: ");
    Serial.print(N);
    Serial.println();

    Serial.print(" P: ");
    Serial.print(P);
    Serial.println();

    Serial.print(" K: ");
    Serial.print(K);
    Serial.println();

    Serial.print(" Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print(" Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    // Prepare JSON payload
    String payload = "{\"N\": " + String(N) + ", \"P\": " + String(P) + ", \"K\": " + String(K) + ", \"temperature\": " + String(temperature) + ", \"humidity\": " + String(humidity) + "}";

    // Send data to Ngrok server
    if (sendDataToNgrok(payload)) {
      Serial.println("Data sent successfully");
    } else {
      Serial.println("Failed to send data");
    }
  }
  delay(100);
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
