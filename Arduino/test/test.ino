#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <HTTPClient.h>

// Insert your network credentials.
#define WIFI_SSID "Wilsonwils"
#define WIFI_PASSWORD "Xzibit5+"

// Insert Ngrok server URL
#define NGROK_URL "https://0d6b-154-161-5-86.ngrok-free.app/api/vehicle/update"

// Define the Digital Pin of the "On Board LED".
#define On_Board_LED 2

// Define the sensor pins
#define TURBIDITY_SENSOR_PIN 34
#define PH_SENSOR_PIN 35

// Millis variable to send/store data to Ngrok server.
unsigned long sendDataPrevMillis = 0;
const long sendDataIntervalMillis = 10000; // Sends/stores data to Ngrok server every 10 seconds.

void setup() {
  Serial.begin(115200);
  Serial.println();

  pinMode(On_Board_LED, OUTPUT);
  pinMode(TURBIDITY_SENSOR_PIN, INPUT);
  pinMode(PH_SENSOR_PIN, INPUT);

  // Connecting the WiFi on the ESP32 to the WiFi Router/Hotspot.
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("---------------Connection");
  Serial.print("Connecting to : ");
  Serial.println(WIFI_SSID);
  
  unsigned long startAttemptTime = millis();

  // Attempt to connect for 10 seconds
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000){
    Serial.print(".");
    digitalWrite(On_Board_LED, HIGH);
    delay(250);
    digitalWrite(On_Board_LED, LOW);
    delay(250);
  }

  // Check if the ESP32 is connected to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println();
    Serial.println("Failed to connect to WiFi. Restarting...");
    ESP.restart();
  }

  digitalWrite(On_Board_LED, LOW);
  Serial.println();
  Serial.print("Successfully connected to : ");
  Serial.println(WIFI_SSID);
  Serial.println("---------------");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED && (millis() - sendDataPrevMillis > sendDataIntervalMillis || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();

    // Read sensor data
    int turbidity_data = analogRead(TURBIDITY_SENSOR_PIN);
    int ph_data = analogRead(PH_SENSOR_PIN);

    Serial.print("Turbidity_data: ");
    Serial.print(turbidity_data);
    Serial.print("\t | pH_data: ");
    Serial.println(ph_data);

    // Prepare JSON payload
    String payload = "{\"turbidity_data\": " + String(turbidity_data) + ", \"ph_data\": " + String(ph_data) + "}";

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
