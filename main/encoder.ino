// --- Encoder Pin Definitions ---
const int encPinA = 2; 
const int encPinB = 3;

// --- Encoder Variables ---
// 'volatile' is required for variables modified inside an ISR
volatile long encCount = 0;
long lastReportedPos = 0;

long getEncCount() { return encCount; }
void setEncCount(long newVal) { encCount=newVal; }

void encoderSetup() {
  // Set up pins with internal pullups 
  // (Encoders usually pull to ground, so we need a default HIGH)
  pinMode(encPinA, INPUT_PULLUP);
  pinMode(encPinB, INPUT_PULLUP);

  // Attach Interrupt to Pin A
  // Trigger on FALLING edge as requested
  attachInterrupt(digitalPinToInterrupt(encPinA), handleEncoder, FALLING);
}

void encoderLoop() {
  // Only print when the value actually changes
  if (encCount != lastReportedPos) {
    Serial.print("Position: ");
    Serial.println(encCount);
    lastReportedPos = encCount;
  }
}

// --- The Encoder ISR ---
// This code runs instantly when Pin A goes from HIGH to LOW
void handleEncoder() {
  // Read Pin B to determine direction
  // If B is HIGH when A falls, we are going one way.
  // If B is LOW when A falls, we are going the other.
  if (digitalRead(encPinB) == HIGH) {
    encCount++;
  } else {
    encCount--;
  }
}