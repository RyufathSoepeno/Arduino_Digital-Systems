// Blink LED on pin 13

int led = 13;

void setup() {
  pinMode(led, OUTPUT);
}

void loop() {
  digitalWrite(led, HIGH); // LED ON
  delay(500);
  digitalWrite(led, LOW);  // LED OFF
  delay(500);
}
