byte ldrPin = A2;
byte ledPin = 13;
byte buzzerPin = 10; // Choose the appropriate pin for your buzzer
int ldrValue;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  ldrValue = analogRead(ldrPin);
  Serial.print("LDR Value: ");
  Serial.println(ldrValue);

  if (ldrValue < 100) {
    // If LDR is hindered, activate LED and buzzer
    digitalWrite(ledPin, HIGH);
    tone(buzzerPin, 1000); // Adjust the frequency as needed
  } else {
    // If LDR is not hindered, turn off LED and buzzer
    digitalWrite(ledPin, LOW);
    noTone(buzzerPin);
  }

  delay(100);  // Adjust the delay time based on your needs
}
