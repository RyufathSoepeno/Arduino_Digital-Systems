// Example: Checking temperature using if statements

void setup() {
  Serial.begin(9600);

  int temp = 30;  // Simulated temperature value

  if (temp > 35) {
    Serial.println("It's too hot!");
  } else if (temp < 20) {
    Serial.println("It's too cold!");
  } else {
    Serial.println("Temperature is normal.");
  }
}

void loop() {
  // Nothing needed here
}
