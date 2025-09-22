/*
  Advanced Arduino MIDI Player
  ----------------------------
  Features:
   - Sends MIDI note messages over 5-pin DIN MIDI OUT
   - Plays a predefined sequence (mini song)
   - Optional piezo buzzer output for monitoring
   - Supports adjustable tempo
   - Well-commented for learning

  MIDI wiring (DIN connector):
    Pin 4 -> Arduino TX (D1) through 220Ω
    Pin 5 -> +5V through 220Ω
    Pin 2 -> GND
*/

#define BUZZER_PIN 8   // optional piezo buzzer pin
#define TEMPO_POT A0   // potentiometer for tempo control

// MIDI baud rate (always 31250 for hardware MIDI)
#define MIDI_BAUD 31250

// Structure for a note event
struct NoteEvent {
  byte pitch;   // MIDI note number (60 = Middle C)
  byte velocity;
  int duration; // in ms
};

// Example sequence (a little C-major scale)
NoteEvent song[] = {
  {60, 100, 400}, // C4
  {62, 100, 400}, // D
  {64, 100, 400}, // E
  {65, 100, 400}, // F
  {67, 100, 400}, // G
  {69, 100, 400}, // A
  {71, 100, 400}, // B
  {72, 100, 800}  // C5
};
int songLength = sizeof(song) / sizeof(song[0]);

// ------------------ FUNCTIONS ----------------------

// Send a MIDI "Note On" message
void midiNoteOn(byte channel, byte pitch, byte velocity) {
  Serial.write(0x90 | (channel & 0x0F)); // 0x90 = Note On
  Serial.write(pitch & 0x7F);
  Serial.write(velocity & 0x7F);
}

// Send a MIDI "Note Off" message
void midiNoteOff(byte channel, byte pitch, byte velocity) {
  Serial.write(0x80 | (channel & 0x0F)); // 0x80 = Note Off
  Serial.write(pitch & 0x7F);
  Serial.write(velocity & 0x7F);
}

// Play a note (both MIDI and optional buzzer)
void playNote(NoteEvent note, int tempoFactor) {
  // Calculate adjusted duration based on tempo
  int adjustedDuration = note.duration * tempoFactor / 100;

  // Send MIDI Note On
  midiNoteOn(0, note.pitch, note.velocity);

  // Also play on buzzer (approx. frequency)
  int freq = 440 * pow(2, (note.pitch - 69) / 12.0);
  tone(BUZZER_PIN, freq, adjustedDuration);

  delay(adjustedDuration);

  // Send MIDI Note Off
  midiNoteOff(0, note.pitch, 0);

  // Short gap between notes
  delay(50);
}

// ------------------- SETUP -------------------------
void setup() {
  Serial.begin(MIDI_BAUD); // MIDI OUT serial speed
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TEMPO_POT, INPUT);

  Serial.println("MIDI Player Ready");
}

// -------------------- LOOP -------------------------
void loop() {
  // Read tempo pot (map 0-1023 to 50%–150% tempo)
  int potVal = analogRead(TEMPO_POT);
  int tempoFactor = map(potVal, 0, 1023, 50, 150);

  // Play the song
  for (int i = 0; i < songLength; i++) {
    playNote(song[i], tempoFactor);
  }

  delay(1000); // pause before repeating
}
