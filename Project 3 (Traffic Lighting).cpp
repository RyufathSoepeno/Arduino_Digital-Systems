/*
  Advanced Traffic Light System for a Roundabout
  ----------------------------------------------
  Features:
   - 4 roads with independent Red, Yellow, Green LEDs
   - Traffic light cycles automatically
   - Pedestrian button: requests crossing, handled safely in cycle
   - Emergency button: all-red or priority lane green
*/

#define NUM_ROADS 4

// ----------------- Pin Definitions -----------------
int redPins[NUM_ROADS]    = {2, 5, 8, 11};
int yellowPins[NUM_ROADS] = {3, 6, 9, 12};
int greenPins[NUM_ROADS]  = {4, 7, 10, 13};

// Pedestrian buttons (1 per road)
int pedButtons[NUM_ROADS] = {A0, A1, A2, A3};

// Emergency override button
#define EMERGENCY_PIN A4

// Optional pedestrian buzzer
#define BUZZER_PIN A5

// ----------------- Timing -----------------
int greenTime   = 5000;  // 5s green
int yellowTime  = 2000;  // 2s yellow
int allRedTime  = 1000;  // 1s all red between changes
int pedTime     = 4000;  // 4s pedestrian crossing

// Pedestrian request flags
bool pedRequest[NUM_ROADS] = {false, false, false, false};

// ----------------- Functions -----------------

// Turn all lights OFF
void allOff() {
  for (int i = 0; i < NUM_ROADS; i++) {
    digitalWrite(redPins[i], LOW);
    digitalWrite(yellowPins[i], LOW);
    digitalWrite(greenPins[i], LOW);
  }
}

// Set a road's light state
void setLights(int road, bool red, bool yellow, bool green) {
  digitalWrite(redPins[road], red);
  digitalWrite(yellowPins[road], yellow);
  digitalWrite(greenPins[road], green);
}

// Pedestrian crossing sequence for given road
void handlePedestrian(int road) {
  if (pedRequest[road]) {
    // Ensure traffic is stopped (all red)
    allOff();
    for (int i = 0; i < NUM_ROADS; i++) digitalWrite(redPins[i], HIGH);

    // Pedestrian buzzer signal
    for (int t = 0; t < pedTime / 500; t++) {
      tone(BUZZER_PIN, 1000, 200);
      delay(500);
    }

    pedRequest[road] = false; // reset request
  }
}

// Emergency mode → all red
void emergencyMode() {
  allOff();
  for (int i = 0; i < NUM_ROADS; i++) {
    digitalWrite(redPins[i], HIGH);
  }
  // Flash buzzer
  for (int i = 0; i < 5; i++) {
    tone(BUZZER_PIN, 200, 200);
    delay(400);
  }
}

// ----------------- Setup -----------------
void setup() {
  // Setup LEDs
  for (int i = 0; i < NUM_ROADS; i++) {
    pinMode(redPins[i], OUTPUT);
    pinMode(yellowPins[i], OUTPUT);
    pinMode(greenPins[i], OUTPUT);
    pinMode(pedButtons[i], INPUT_PULLUP); // button active LOW
  }
  pinMode(EMERGENCY_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  allOff();
}

// ----------------- Main Loop -----------------
void loop() {
  // Check pedestrian buttons
  for (int i = 0; i < NUM_ROADS; i++) {
    if (digitalRead(pedButtons[i]) == LOW) {
      pedRequest[i] = true;
    }
  }

  // Check emergency button
  if (digitalRead(EMERGENCY_PIN) == LOW) {
    emergencyMode();
    return; // skip normal cycle until released
  }

  // Cycle through each road
  for (int road = 0; road < NUM_ROADS; road++) {
    // Green for this road
    setLights(road, LOW, LOW, HIGH);
    delay(greenTime);

    // Handle pedestrian crossing if requested
    handlePedestrian(road);

    // Yellow phase
    setLights(road, LOW, HIGH, LOW);
    delay(yellowTime);

    // All red before next road
    allOff();
    for (int i = 0; i < NUM_ROADS; i++) {
      digitalWrite(redPins[i], HIGH);
    }
    delay(allRedTime);
  }
}
