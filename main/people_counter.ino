// People Counter Module
// Detects entry/exit using ToF sensors and maintains occupancy count

// Counter variables
volatile int peopleIn = 0;        // Current occupancy
volatile int totalPeople = 0;     // Cumulative count for the day

// Detection state tracking
volatile int lastD1 = 0;
volatile int lastD2 = 0;
volatile int lastD3 = 0;

// Configuration
#define DOOR_DISTANCE_THRESHOLD 800  // mm - adjust based on your doorway
#define HYSTERESIS 50                // mm - prevents jitter
#define DETECTION_TIMEOUT 2000       // ms - time window for entry/exit

volatile unsigned long lastDetectionTime = 0;
volatile bool detectionInProgress = false;

void counterSetup() {
  Serial.println("People Counter initialized");
}

void updateCounter(int d1, int d2, int d3) {
  // Check if we have valid readings (0 means invalid)
  if (d1 == 0 || d2 == 0 || d3 == 0) return;

  unsigned long currentTime = millis();
  
  // Check if person is detected (any sensor below threshold)
  bool d1Detected = (d1 < DOOR_DISTANCE_THRESHOLD);
  bool d2Detected = (d2 < DOOR_DISTANCE_THRESHOLD);
  bool d3Detected = (d3 < DOOR_DISTANCE_THRESHOLD);
  
  int detectedCount = (d1Detected ? 1 : 0) + (d2Detected ? 1 : 0) + (d3Detected ? 1 : 0);
  
  // High confidence detection: 2+ sensors triggered
  if (detectedCount >= 2) {
    if (!detectionInProgress) {
      detectionInProgress = true;
      lastDetectionTime = currentTime;
      
      // Determine direction based on which sensors are triggered
      // Front sensor (d1) closer = entering
      // Back sensor (d3) closer = exiting
      if (d1Detected && !d3Detected) {
        // Person entering
        peopleIn++;
        totalPeople++;
        Serial.print("ENTRY DETECTED - People in room: ");
        Serial.print(peopleIn);
        Serial.print(", Total: ");
        Serial.println(totalPeople);
      } 
      else if (d3Detected && !d1Detected) {
        // Person exiting
        if (peopleIn > 0) {
          peopleIn--;
        }
        Serial.print("EXIT DETECTED - People in room: ");
        Serial.print(peopleIn);
        Serial.print(", Total: ");
        Serial.println(totalPeople);
      }
      else if (d1Detected && d3Detected) {
        // Both ends triggered - ambiguous, default to entry
        peopleIn++;
        totalPeople++;
        Serial.print("AMBIGUOUS DETECTION - Counted as entry. People in room: ");
        Serial.println(peopleIn);
      }
    }
  } 
  else if (detectionInProgress && (currentTime - lastDetectionTime) > DETECTION_TIMEOUT) {
    // Reset detection after timeout
    detectionInProgress = false;
  }
  
  // Store current readings
  lastD1 = d1;
  lastD2 = d2;
  lastD3 = d3;
}

void resetCounter() {
  peopleIn = 0;
  totalPeople = 0;
  detectionInProgress = false;
  Serial.println("Counter reset!");
  printCounterStatus();
}

int getPeopleInRoom() {
  return peopleIn;
}

int getTotalPeopleCount() {
  return totalPeople;
}

void printCounterStatus() {
  Serial.print("=== COUNTER STATUS ===");
  Serial.print(" | People in room: ");
  Serial.print(peopleIn);
  Serial.print(" | Total today: ");
  Serial.println(totalPeople);
}
