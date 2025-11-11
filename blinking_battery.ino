// Three LEDs with button control

int led1 = 2;
int led2 = 3;
int led3 = 4;
int buttonPin = 7;
int buttonState = 0; // Tip: Meaning it is off, 1 when it's on: Binary code

void setup() {
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(buttonPin, INPUT);
}

void loop() {
  buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH) {
    // If button is pressed → blink LEDs in sequence
    digitalWrite(led1, HIGH);
    delay(300);
    digitalWrite(led1, LOW);

    digitalWrite(led2, HIGH);
    delay(300);
    digitalWrite(led2, LOW);

    digitalWrite(led3, HIGH);
    delay(300);
    digitalWrite(led3, LOW);
  } 
  else {
    // If button is not pressed → keep LEDs off
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
  }
}
