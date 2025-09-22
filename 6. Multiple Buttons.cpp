// Two buttons control two LEDs

int button1 = 2;
int button2 = 3;
int led1 = 8;
int led2 = 9;

void setup() {
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
}

void loop() {
  if (digitalRead(button1) == HIGH) {
    digitalWrite(led1, HIGH);
  } else {
    digitalWrite(led1, LOW);
  }

  if (digitalRead(button2) == HIGH) {
    digitalWrite(led2, HIGH);
  } else {
    digitalWrite(led2, LOW);
  }
}
