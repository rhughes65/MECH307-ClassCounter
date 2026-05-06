// Pins for ULN2003
const int motorPins[] = {10, 11, 12, 13};

// Volatile variables for ISR
volatile int  mStepIdx = 0;
volatile int  mDir = 1; 
volatile bool mRunning = false;
volatile long stepsToTake = 0; // The "Step Counter"

// 4-step sequence
const byte mSeq[4] = { 0b1000, 0b0100, 0b0010, 0b0001 };

void motorSetup() {
  for (int i = 0; i < 4; i++) pinMode(motorPins[i], OUTPUT);

  // --- TIMER 1 SETUP ---
  cli();
  TCCR1A = 0; TCCR1B = 0; TCNT1 = 0;
  OCR1A = 609; // 12 RPM @ 2048 steps/rev
  TCCR1B |= (1 << WGM12); 
  TCCR1B |= (1 << CS11) | (1 << CS10); // 64 Prescaler
  TIMSK1 |= (1 << OCIE1A);
  sei();
}

// --- The New "Library-Style" Function ---
void step(long steps) {
  if (steps == 0) return;

  // Set direction based on positive/negative input
  mDir = (steps > 0) ? 1 : -1;
  
  // Set the counter (use absolute value)
  stepsToTake = (steps > 0) ? steps : -steps;
  
  mRunning = true;
}

void stopMotor() {
  mRunning = false;
  stepsToTake = 0;
  // Safety: De-energize coils
  for (int i = 0; i < 4; i++) digitalWrite(motorPins[i], LOW);
}

// --- The ISR ---
ISR(TIMER1_COMPA_vect) {
  if (!mRunning) return;

  if (stepsToTake > 0) {
    // 1. Move the motor
    for (int i = 0; i < 4; i++) {
      digitalWrite(motorPins[i], (mSeq[mStepIdx] >> i) & 0x01);
    }

    // 2. Update the index
    mStepIdx += mDir;
    if (mStepIdx > 3) mStepIdx = 0;
    if (mStepIdx < 0) mStepIdx = 3;

    // 3. Decrement the counter
    stepsToTake--;
  } else {
    // 4. Reach zero? Shutdown.
    stopMotor();
  }
}
