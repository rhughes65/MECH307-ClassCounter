#include <HCSR04.h>

byte triggerPin = 8;
byte echoPin = 9;

#define WINDOW_SIZE 3
double distances[WINDOW_SIZE];
int indexer = 0;

double previousMedian = 0;
bool hasPrevious = false;

const double ARM_THRESHOLD = 100.0;
const double TRIGGER_THRESHOLD = 50.0;

unsigned long lastTriggerTime = 0;
const unsigned long COOLDOWN = 3000;

int counter = 0;
bool armed = false;

void setup () {
  Serial.begin(9600);
  HCSR04.begin(triggerPin, echoPin);
}

void sort(double a[], int n) {
  for (int i = 0; i < n - 1; ++i) {
    for (int j = 0; j < n - 1 - i; ++j) {
      if (a[j] > a[j + 1]) {
        double temp = a[j];
        a[j] = a[j + 1];
        a[j + 1] = temp;
      }
    }
  }
}

void loop () {
  double* readingPtr = HCSR04.measureDistanceCm();
  double currentReading = *readingPtr;

  if (currentReading > 0) { 
    distances[indexer] = currentReading;
    indexer++;
  }

  double sortBuffer[WINDOW_SIZE];
  for(int i = 0; i < WINDOW_SIZE; i++) {
    sortBuffer[i] = distances[i];
  }

  sort(sortBuffer, WINDOW_SIZE);
  double median = sortBuffer[WINDOW_SIZE / 2];

  unsigned long now = millis();

  // --- ARMING LOGIC ---
  if (median > ARM_THRESHOLD) {
    armed = true;
  }

  // --- FALLING EDGE DETECTION WITH ARMING + COOLDOWN ---
  if (hasPrevious && armed) {
    bool fallingEdge = (previousMedian > TRIGGER_THRESHOLD && median <= TRIGGER_THRESHOLD);
    bool cooldownOver = (now - lastTriggerTime >= COOLDOWN);

    if (fallingEdge && cooldownOver) {
      counter++;
      lastTriggerTime = now;
      armed = false; // disarm so it doesn't retrigger
    }
  }

  previousMedian = median;
  hasPrevious = true;

  // --- OUTPUT ---
  Serial.print("Filtered:");
  Serial.print(median);
  Serial.print(",Raw:");
  Serial.print(currentReading);
  Serial.print(",Count:");
  Serial.println(counter);

  if (indexer >= WINDOW_SIZE) {
    indexer = 0; 
  }

  delay(60); 
}
