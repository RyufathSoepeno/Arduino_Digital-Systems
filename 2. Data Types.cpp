// Demonstration of Arduino Data Types

void setup() {
  Serial.begin(9600);

  int age = 20;                // Integer type
  float temperature = 36.5;    // Floating-point type
  char grade = 'A';            // Character type
  String name = "Arduino";     // String type
  boolean isOn = true;         // Boolean type (true/false)

  // Print each variable
  Serial.println(age);
  Serial.println(temperature);
  Serial.println(grade);
  Serial.println(name);
  Serial.println(isOn);
}

void loop() {
  // Nothing in loop, values are printed once
}
