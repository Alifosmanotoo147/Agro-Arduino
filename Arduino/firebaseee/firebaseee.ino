//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Information about Sign-up quota.
//
// Until this video or project was created, to access the Firebase Realtime Database there is a Sign-up quota. Here's an explanation:
//
//  __________________________________________________________________________________________________________________________________________________________________________
// | Sign-up quota                                                                                                                                                            |
// |                                                                                                                                                                          |
// | To protect your project from abuse, we limit the number of new Email/Password and Anonymous accounts that your application can create in a day from a single IP address. |
// |                                                                                                                                                                          |
// | Current quota per hour: 100                                                                                                                                              |
// |__________________________________________________________________________________________________________________________________________________________________________|
//
// For more information, you can see it at:
// (Open your Firebase project) Authentication -> click the Settings tab -> click Sign-up quota.
// Or you can also see it here: https://firebase.google.com/docs/auth/limits
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<



//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 01_Store_Data
/*
 *  Reference : https://randomnerdtutorials.com/esp32-firebase-realtime-database/
 */

//======================================== Including the libraries.
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
//======================================== 

//======================================== Insert your network credentials.
#define WIFI_SSID "Galaxy A02s1685"
#define WIFI_PASSWORD "gikk3536"
// gikk3536 password
//Galaxy A02s1685
//======================================== 

//Provide the token generation process info.
#include "addons/TokenHelper.h"

//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Defines the Digital Pin of the "On Board LED".
#define On_Board_LED 2

// Insert Firebase project API Key
#define API_KEY "AIzaSyDa7Dm9Ep_93tIefOidVb1Lf1ALBHI8VoA"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://agropredict-143e6-default-rtdb.firebaseio.com/" 

// Define Firebase Data object.
FirebaseData fbdo;

// Define firebase authentication.
FirebaseAuth auth;

// Definee firebase configuration.
FirebaseConfig config;

//======================================== Millis variable to send/store data to firebase database.
unsigned long sendDataPrevMillis = 0;
const long sendDataIntervalMillis = 10000; //--> Sends/stores data to firebase database every 10 seconds.
//======================================== 

// Boolean variable for sign in status.
bool signupOK = false;

float store_random_Float_Val;
int store_random_Int_Val;

//________________________________________________________________________________ VOID SETUP
void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(115200);
  Serial.println();

  pinMode(On_Board_LED, OUTPUT);

  //---------------------------------------- The process of connecting the WiFi on the ESP32 to the WiFi Router/Hotspot.
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
  //Serial.print("IP : ");
  //Serial.println(WiFi.localIP());
  Serial.println("---------------");
  //---------------------------------------- 

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
  config.token_status_callback = tokenStatusCallback; //--> see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}
//________________________________________________________________________________ 

//________________________________________________________________________________ VOID LOOP
void loop() {
  // put your main code here, to run repeatedly:
  
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > sendDataIntervalMillis || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();

    //---------------------------------------- Generate random values.
    int randNumber = random(15, 40);
    float f = (float)randNumber / 1.01;
    int i = (int(f*100));
    store_random_Float_Val = float(i) / 100;
    store_random_Int_Val = random(10, 99);
    //---------------------------------------- 

    //---------------------------------------- 
    Serial.println();
    Serial.println("---------------Random Value");
    Serial.print("Random Float_Val : ");
    Serial.println(store_random_Float_Val);
    Serial.print("Random Int_Val   : ");
    Serial.println(store_random_Int_Val);
    Serial.println("---------------");
    //---------------------------------------- 

    //---------------------------------------- The process of sending/storing data to the firebase database.
    Serial.println();
    Serial.println("---------------Store Data");
    digitalWrite(On_Board_LED, HIGH);
    
    // Write an Int number on the database path test/random_Float_Val.
    if (Firebase.RTDB.setFloat(&fbdo, "Test/random_Float_Val", store_random_Float_Val)) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    
    // Write an Float number on the database path test/random_Int_Val.
    if (Firebase.RTDB.setInt(&fbdo, "Test/random_Int_Val", store_random_Int_Val)) {
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
    //---------------------------------------- 
  }
}
//________________________________________________________________________________ 
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


