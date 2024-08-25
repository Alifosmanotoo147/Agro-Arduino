#include <Wire.h>

#define NPK_SENSOR_ADDR 34  // Replace with your NPK sensor's I2C address

void setup() {
  Serial.begin(115200);  // Initialize serial communication
  Wire.begin();          // Initialize I2C communication
  Serial.println("NPK Sensor Initialization");

  // Test if the sensor is connected
  Wire.beginTransmission(NPK_SENSOR_ADDR);
  if (Wire.endTransmission() == 0) {
    Serial.println("NPK sensor detected");
  } else {
    Serial.println("NPK sensor not detected");
  }
}

void loop() {
  Wire.beginTransmission(NPK_SENSOR_ADDR);
  Wire.write(0x00);  // Command to request NPK data, may differ based on your sensor
  Wire.endTransmission();

  Wire.requestFrom(NPK_SENSOR_ADDR, 6);  // Request 6 bytes from the sensor (N, P, K values, 2 bytes each)

  if (Wire.available() == 6) {
    int nitrogen = Wire.read() << 1 | Wire.read();  // Combine two bytes for nitrogen value
    int phosphorus = Wire.read() << 1 | Wire.read();  // Combine two bytes for phosphorus value
    int potassium = Wire.read() << 1 | Wire.read();  // Combine two bytes for potassium value
    
    // Print NPK values to the serial monitor
    Serial.print("Nitrogen: ");
    Serial.print(nitrogen);
    Serial.print(" mg/kg, Phosphorus: ");
    Serial.print(phosphorus);
    Serial.print(" mg/kg, Potassium: ");
    Serial.println(potassium);
  } else {
    Serial.println("Failed to read from NPK sensor");
    Serial.print("Bytes available: ");
    Serial.println(Wire.available());
  }

  delay(2000);  // Wait for 2 seconds before reading again
}
