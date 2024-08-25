//======================================== Including the libraries.
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

// Provide the token generation process info.
#include "addons/TokenHelper.h"

// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials.
#define WIFI_SSID "Galaxy A02s1685"
#define WIFI_PASSWORD "gikk3536"

// Insert Firebase project API Key
#define API_KEY "AIzaSyDa7Dm9Ep_93tIefOidVb1Lf1ALBHI8VoA"

// Insert RTDB URL
#define DATABASE_URL "https://agropredict-143e6-default-rtdb.firebaseio.com/" 

// Define Firebase Data object.
FirebaseData fbdo;

// Define firebase authentication.
FirebaseAuth auth;

// Define firebase configuration.
FirebaseConfig config;

// Define the Digital Pin of the "On Board LED".
#define On_Board_LED 2

// Define the sensor pin
int sensor_pin = 35;

// Millis variable to send/store data to firebase database.
unsigned long sendDataPrevMillis = 0;
const long sendDataIntervalMillis = 10000; // Sends/stores data to firebase database every 10 seconds.

// Boolean variable for sign in status.
bool signupOK = false;

void setup() {
  Serial.begin(115200);
  Serial.println();

  pinMode(On_Board_LED, OUTPUT);
  pinMode(sensor_pin, INPUT);

  // Connecting the WiFi on the ESP32 to the WiFi Router/Hotspot.
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("---------------Connection");
  Serial.print("Connecting to : ");
  Serial.println(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    digitalWrite(On_Board_LED, HIGH);
    delay(250);
    digitalWrite(On_Board_LED, LOW);
    delay(250);
  }
  digitalWrite(On_Board_LED, LOW);
  Serial.println();
  Serial.print("Successfully connected to : ");
  Serial.println(WIFI_SSID);
  Serial.println("---------------");

  // Assign the api key (required).
  config.api_key = API_KEY;

  // Assign the RTDB URL (required).
  config.database_url = DATABASE_URL;

  // Sign up.
  Serial.println();
  Serial.println("---------------Sign up");
  Serial.print("Sign up new user... ");
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  Serial.println("---------------");
  
  // Assign the callback function for the long running token generation task.
  config.token_status_callback = tokenStatusCallback; // See addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > sendDataIntervalMillis || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();

    // Read sensor data
    int sensor_data = analogRead(sensor_pin);
    Serial.print("Sensor_data: ");
    Serial.print(sensor_data);
    Serial.print("\t | ");
    String soil_status;

    if(sensor_data > 3600) {
      soil_status = "No moisture, Soil is dry";
    } else if(sensor_data >= 2400 && sensor_data <= 3600) {
      soil_status = "There is some moisture, Soil is medium";
    } else if(sensor_data < 2400) {
      soil_status = "Soil is wet";
    }
    Serial.println(soil_status);

    // Sending/storing data to the firebase database.
    Serial.println();
    Serial.println("---------------Store Data");
    digitalWrite(On_Board_LED, HIGH);

    // Write sensor data on the database path
    if (Firebase.RTDB.setInt(&fbdo, "Sensor/sensor_data", sensor_data)) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    // Write soil status on the database path
    if (Firebase.RTDB.setString(&fbdo, "Sensor/soil_status", soil_status)) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    digitalWrite(On_Board_LED, LOW);
    Serial.println("---------------");
  }
  delay(100);
}
