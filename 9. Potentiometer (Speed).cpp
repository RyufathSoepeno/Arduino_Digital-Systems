// Speedometer using potentiometer input

int potPin = A0;   // Potentiometer pin
int speedValue = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  speedValue = analogRead(potPin);  // Read potentiometer (0-1023)
  int speed = map(speedValue, 0, 1023, 0, 200); // Map to 0-200 km/h
  Serial.print("Speed: ");
  Serial.print(speed);
  Serial.println(" km/h");
  delay(200);
}
