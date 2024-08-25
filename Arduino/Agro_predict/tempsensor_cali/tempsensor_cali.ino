#include <DHT.h>

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int sensorValue = analogRead(34);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = sensorValue * (0.15 / 1023.0);
  float temperature = voltage*100;
  // print out the value you read:
  Serial.println("Voltage : ");
  Serial.println(voltage);
  Serial.println("Temperature : ");
  Serial.println(temperature);
  delay(500);
}