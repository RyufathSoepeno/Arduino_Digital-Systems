// Pin assignments
const int ledLeft = 2;
const int ledMiddle = 3;
const int ledRight = 4;

const int buttonLeft = 5;
const int buttonMiddle = 6;
const int buttonRight = 7;



// Tip: Use 10k resistors for components with high energy (i.e. buttons)

// Tip 2: GND and pins can flow together


void setup() {
  // LEDs as outputs
  pinMode(ledLeft, OUTPUT);
  pinMode(ledMiddle, OUTPUT);
  pinMode(ledRight, OUTPUT);

  // Buttons as inputs
  pinMode(buttonLeft, INPUT);
  pinMode(buttonMiddle, INPUT);
  pinMode(buttonRight, INPUT);
}




void loop() {
  // Read button states
  int leftPressed = digitalRead(buttonLeft);
  int middlePressed = digitalRead(buttonMiddle);
  int rightPressed = digitalRead(buttonRight);

  // LEFT button: blink left to right
  if (leftPressed == HIGH) {
    digitalWrite(ledLeft, HIGH);
    delay(200);
    digitalWrite(ledLeft, LOW);
    digitalWrite(ledMiddle, HIGH);
    delay(200);
    digitalWrite(ledMiddle, LOW);
    digitalWrite(ledRight, HIGH);
    delay(200);
    digitalWrite(ledRight, LOW);
  }

  // MIDDLE button: blink all together
  else if (middlePressed == HIGH) {
    digitalWrite(ledLeft, HIGH);
    digitalWrite(ledMiddle, HIGH);
    digitalWrite(ledRight, HIGH);
    delay(300);
    digitalWrite(ledLeft, LOW);
    digitalWrite(ledMiddle, LOW);
    digitalWrite(ledRight, LOW);
    delay(300);
  }

  // RIGHT button: blink right to left
  else if (rightPressed == HIGH) {
    digitalWrite(ledRight, HIGH);
    delay(200);
    digitalWrite(ledRight, LOW);
    digitalWrite(ledMiddle, HIGH);
    delay(200);
    digitalWrite(ledMiddle, LOW);
    digitalWrite(ledLeft, HIGH);
    delay(200);
    digitalWrite(ledLeft, LOW);
  }

  // If no button pressed, keep LEDs off
  else {
    digitalWrite(ledLeft, LOW);
    digitalWrite(ledMiddle, LOW);
    digitalWrite(ledRight, LOW);
  }
}
