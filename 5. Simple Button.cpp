// Button turns on LED

int buttonPin = 2;   // Button connected to pin 2
int ledPin = 13;     // LED on pin 13

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  int state = digitalRead(buttonPin); // Read button
  if (state == HIGH) {
    digitalWrite(ledPin, HIGH);  // LED ON
  } else {
    digitalWrite(ledPin, LOW);   // LED OFF
  }
}
