// Hello World on Arduino (Serial Monitor)

// The setup() function runs once at the beginning
void setup() {
  // Start Serial Communication at 9600 baud rate
  Serial.begin(9600);  
}

// The loop() function runs continuously
void loop() {
  // Print "Hello, World!" to Serial Monitor
  Serial.println("Hello, World!");  

  // Wait 1 second before repeating
  delay(1000);  
}
