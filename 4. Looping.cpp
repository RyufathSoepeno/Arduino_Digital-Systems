// Loop examples: for, while, do-while

void setup() {
  Serial.begin(9600);

  // For loop: counts from 1 to 5
  for (int i = 1; i <= 5; i++) {
    Serial.print("For Loop Count: ");
    Serial.println(i);
  }

  // While loop: counts down
  int j = 5;
  while (j > 0) {
    Serial.print("While Loop Count: ");
    Serial.println(j);
    j--;
  }

  // Do-while loop: executes at least once
  int k = 0;
  do {
    Serial.print("Do-While Loop Count: ");
    Serial.println(k);
    k++;
  } while (k < 3);
}

void loop() {
  // Empty loop
}
