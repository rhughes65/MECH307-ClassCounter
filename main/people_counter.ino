// People Counter Module
// Detects entry/exit using ToF sensors and maintains occupancy count

#ifndef NUM_TOF_SENSORS
#define NUM_TOF_SENSORS 3
#endif

// Counter variables
volatile int peopleIn = 0;        // Current occupancy
volatile int totalPeople = 0;     // Cumulative count for the day

// Filtering Configuration
#define WINDOW_SIZE 3
int distances1[WINDOW_SIZE];
int distances2[WINDOW_SIZE];
int distances3[WINDOW_SIZE];
int idx1 = 0, idx2 = 0, idx3 = 0;

// Logic Thresholds (adjust these based on your doorway)
#define ARM_THRESHOLD 1500     // Distance > this means "clear" (falling from ~2000)
#define TRIGGER_THRESHOLD 1000   // Distance <= this means "person detected"
#define COOLDOWN 1500           // ms between counts
#define MOTOR_STEPS_PER_EVENT 530 // Rotation per entry/exit

// State tracking
bool armed1 = false;
bool armed2 = false;
bool armed3 = false;
unsigned long lastTriggerTime = 0;
int lastMedian1 = 0;
int lastMedian2 = 0;
int lastMedian3 = 0;

void counterSetup() {
  // Initialize filter buffers with a "clear" value
  for(int i=0; i<WINDOW_SIZE; i++) {
    distances1[i] = distances2[i] = distances3[i] = ARM_THRESHOLD + 100;
  }
  Serial.println("People Counter initialized with Median Filtering");
}

// Simple bubble sort for median calculation
void sortDistances(int a[], int n) {
  for (int i = 0; i < n - 1; ++i) {
    for (int j = 0; j < n - 1 - i; ++j) {
      if (a[j] > a[j + 1]) {
        int temp = a[j];
        a[j] = a[j + 1];
        a[j + 1] = temp;
      }
    }
  }
}

int getMedian(int rawValue, int buffer[], int &idx) {
  if (rawValue > 0) {
    buffer[idx] = rawValue;
    idx = (idx + 1) % WINDOW_SIZE;
  }
  
  int sortBuffer[WINDOW_SIZE];
  for(int i = 0; i < WINDOW_SIZE; i++) sortBuffer[i] = buffer[i];
  sortDistances(sortBuffer, WINDOW_SIZE);
  return sortBuffer[WINDOW_SIZE / 2];
}

void updateCounter(int d1, int d2, int d3) {
  unsigned long now = millis();
  
  // 1. Get filtered values
  int m1 = getMedian(d1, distances1, idx1);
  int m2 = getMedian(d2, distances2, idx2);
  int m3 = getMedian(d3, distances3, idx3);

  // 2. Logic based on number of sensors
  #if NUM_TOF_SENSORS == 1
  // --- SINGLE SENSOR FALLING EDGE LOGIC ---
  
  // Arming: distance goes high (door is clear)
  if (m1 > ARM_THRESHOLD) {
    armed1 = true;
  }

  // Trigger: distance falls below threshold while armed
  bool fallingEdge = (lastMedian1 > TRIGGER_THRESHOLD && m1 <= TRIGGER_THRESHOLD);
  bool cooldownOver = (now - lastTriggerTime >= COOLDOWN);

  if (armed1 && fallingEdge && cooldownOver) {
    peopleIn++;
    totalPeople++;
    lastTriggerTime = now;
    armed1 = false; // Disarm until clear again
    
    Serial.print("DETECTION (Single Sensor) - People in room: ");
    Serial.println(peopleIn);
  }
  
  #else
  // --- MULTI-SENSOR DIRECTIONAL LOGIC ---
  // (Uses similar arming principle but looks for sequences)
  
  if (m1 > ARM_THRESHOLD) armed1 = true;
  if (m2 > ARM_THRESHOLD) armed2 = true;
  if (m3 > ARM_THRESHOLD) armed3 = true;

  bool fallingEdge1 = (lastMedian1 > TRIGGER_THRESHOLD && m1 <= TRIGGER_THRESHOLD);
  bool fallingEdge2 = (lastMedian2 > TRIGGER_THRESHOLD && m2 <= TRIGGER_THRESHOLD);
  bool fallingEdge3 = (lastMedian3 > TRIGGER_THRESHOLD && m3 <= TRIGGER_THRESHOLD);
  
  if (fallingEdge1) Serial.println("FE1");
  if (fallingEdge2) Serial.println("FE2");
  if (fallingEdge3) Serial.println("FE3");

  bool cooldownOver = (now - lastTriggerTime >= COOLDOWN);

  if (cooldownOver) {
    #if NUM_TOF_SENSORS == 2
    // 2 Sensors: Check if sensor 1 triggers before sensor 2
    if (armed1 && fallingEdge1 && armed2) {
      peopleIn++; totalPeople++;
      lastTriggerTime = now;
      armed1 = armed2 = false;
      Serial.println("ENTRY (S1->S2)");
      step(-MOTOR_STEPS_PER_EVENT); // Doubled: 164 * 2
    } else if (armed2 && fallingEdge2 && armed1) {
      if (peopleIn > 0) peopleIn--;
      lastTriggerTime = now;
      armed1 = armed2 = false;
      Serial.println("EXIT (S2->S1)");
      step(MOTOR_STEPS_PER_EVENT); // Doubled: 164 * 2
    }
    #else
    // 3 Sensors: More robust directional detection
    if (armed1 && fallingEdge1 && armed3) {
      peopleIn++; totalPeople++;
      lastTriggerTime = now;
      armed1 = armed2 = armed3 = false;
      Serial.println("ENTRY (S1 lead)");
      step(-MOTOR_STEPS_PER_EVENT); // Doubled: 164 * 2
    } else if (armed3 && fallingEdge3 && armed1) {
      if (peopleIn > 0) peopleIn--;
      lastTriggerTime = now;
      armed1 = armed2 = armed3 = false;
      Serial.println("EXIT (S3 lead)");
      step(MOTOR_STEPS_PER_EVENT); // Doubled: 164 * 2
    }
    #endif
  }
  #endif

  // Store for next iteration
  lastMedian1 = m1;
  lastMedian2 = m2;
  lastMedian3 = m3;
}

void resetCounter() {
  peopleIn = 0;
  totalPeople = 0;
  armed1 = armed2 = armed3 = false;
  Serial.println("Counter reset!");
}

int getPeopleInRoom() { return peopleIn; }
int getTotalPeopleCount() { return totalPeople; }

void printCounterStatus() {
  Serial.print("Occupancy: ");
  Serial.print(peopleIn);
  Serial.print(" | Total: ");
  Serial.println(totalPeople);
}
