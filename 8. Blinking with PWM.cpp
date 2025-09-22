 // Pulse-Width Modulation, is a technique for controlling the average power or amplitude of an electrical signal by varying the "on-time" (pulse width) of a rectangular wave within a fixed period
 // Basically similar to squared analog wavelenghts

int led = 9;  // Must be a PWM pin

void setup() {
  pinMode(led, OUTPUT);
}

void loop() {
  // Fade in
  for (int i = 0; i <= 255; i++) {
    analogWrite(led, i);
    delay(10);
  }
  // Fade out
  for (int i = 255; i >= 0; i--) {
    analogWrite(led, i);
    delay(10);
  }
}
