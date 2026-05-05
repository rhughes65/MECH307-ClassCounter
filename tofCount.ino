#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// --- Configuration ---
const int DETECTION_THRESHOLD = 1000; // Distance in mm (e.g., 1 meter)
const int DEBOUNCE_DELAY = 50;        // Small delay to stabilize readings

int personCount = 0;
bool isPersonDetected = false;

void setup() {
  Serial.begin(115200);

  while (!Serial) {
    delay(1);
  }
  
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
  
  Serial.println(F("Person Counter Initialized..."));
  Serial.print("Current Count: ");
  Serial.println(personCount);
}

void loop() {
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false); 

  // Check if the reading is valid (RangeStatus 0 is a clean hit)
  if (measure.RangeStatus != 4) {
    int currentDistance = measure.RangeMilliMeter;

    // Logic: If distance is less than threshold and we weren't already detecting someone
    if (currentDistance < DETECTION_THRESHOLD && !isPersonDetected) {
      isPersonDetected = true;
      personCount++;
      
      Serial.print("Person Detected! Total Count: ");
      Serial.println(personCount);
    } 
    // Logic: If distance returns to normal, reset the detection state
    else if (currentDistance >= DETECTION_THRESHOLD && isPersonDetected) {
      isPersonDetected = false;
      Serial.println("Path Clear.");
    }
  }

  delay(DEBOUNCE_DELAY);
}
