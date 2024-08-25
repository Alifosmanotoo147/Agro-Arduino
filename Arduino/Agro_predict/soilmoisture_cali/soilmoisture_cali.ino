int sensor_pin = 35;

void setup() {
  Serial.begin(9600);
  pinMode(sensor_pin, INPUT);
}

void loop() {
  int sensor_data = analogRead(sensor_pin);
  Serial.print("Sensor_data: ");
  Serial.print(sensor_data);
  Serial.print("\t | ");

  if(sensor_data > 3600) {
    Serial.println("No moisture, Soil is dry");
  } else if(sensor_data >= 2400 && sensor_data <= 3600) {
    Serial.println("There is some moisture, Soil is medium");
  } else if(sensor_data < 2400) {
    Serial.println("Soil is wet");
  }

  delay(100);
}
