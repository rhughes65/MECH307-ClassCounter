// Pins for ULN2003
const int motorPins[] = {10, 11, 12, 13};

// Volatile variables for ISR communication
volatile int stepIndex = 0;
volatile int motorDirection = 1;  // 1 for Forward, -1 for Reverse
volatile bool isMoving = false;    // Control state

// 4-step sequence for 28BYJ-48
const byte motorSequence[4] = {
  0b1000, 0b0100, 0b0010, 0b0001 
};

void motorSetup() {
  for (int i = 0; i < 4; i++) pinMode(motorPins[i], OUTPUT);

  // --- TIMER 1 SETUP ---
  cli();
  TCCR1A = 0; TCCR1B = 0; TCNT1 = 0;
  OCR1A = 609; // 12 RPM
  TCCR1B |= (1 << WGM12); 
  TCCR1B |= (1 << CS11) | (1 << CS10); // 64 Prescaler
  TIMSK1 |= (1 << OCIE1A);
  sei();
}

// --- Control Functions ---

void stopMotor() {
  isMoving = false;
  // Safety: Turn off all coils to prevent overheating
  for (int i = 0; i < 4; i++) digitalWrite(motorPins[i], LOW);
}

void startForward() {
  motorDirection = 1;
  isMoving = true;
}

void startReverse() {
  motorDirection = -1;
  isMoving = true;
}

// --- The ISR ---
ISR(TIMER1_COMPA_vect) {
  if (!isMoving) return; // Do nothing if motor is stopped

  // Apply the bit pattern
  for (int i = 0; i < 4; i++) {
    digitalWrite(motorPins[i], (motorSequence[stepIndex] >> i) & 0x01);
  }
  
  // Calculate next step index
  stepIndex += motorDirection;

  // Handle wrap-around for both directions
  if (stepIndex > 3) stepIndex = 0;
  if (stepIndex < 0) stepIndex = 3;
}
