// constants won't change. They're used here to set pin numbers:
const int BUTTON_PIN_1 = 7;  // the number of the first pushbutton pin
const int BUTTON_PIN_2 = 6;  // the number of the second pushbutton pin
const int LED_PIN = 3;       // the number of the LED pin

// variables will change:
int buttonState1 = 0;   // variable for reading the first pushbutton status
int buttonState2 = 0;   // variable for reading the second pushbutton status

void setup() {
  // initialize the LED pin as an output:
  pinMode(LED_PIN, OUTPUT);
  // initialize the first pushbutton pin as a pull-up input:
  pinMode(BUTTON_PIN_1, INPUT_PULLUP);
  // initialize the second pushbutton pin as a pull-up input:
  pinMode(BUTTON_PIN_2, INPUT_PULLUP);
}

void loop() {
  // read the state of the first pushbutton value:
  buttonState1 = digitalRead(BUTTON_PIN_1);
  // read the state of the second pushbutton value:
  buttonState2 = digitalRead(BUTTON_PIN_2);

  // control LED according to the state of the buttons
  if (buttonState1 == LOW || buttonState2 == LOW) {
    // If either button is pressed, turn on the LED
    digitalWrite(LED_PIN, HIGH);
  } else {
    // If neither button is pressed, turn off the LED
    digitalWrite(LED_PIN, LOW);
  }
}
