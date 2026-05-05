#include <HCSR04.h>

byte triggerPin = 8;
byte echoPin = 9;

#define WINDOW_SIZE 3 // stores 3 distance readings at a time
double distances[WINDOW_SIZE]; // creates an array that can hold 3 readings
int indexer = 0; // keeps track of which spot in distances array should be overwritten next

double previousMedian = 0; // stores previous filtered distance
bool hasPrevious = false;

const double ARM_THRESHOLD = 100.0; // System arms when object is farther than 100 cm
const double TRIGGER_THRESHOLD = 50.0; // Counts when filtered distance crosses 50 cm or less

unsigned long lastTriggerTime = 0; // Stores the last time a count happened
const unsigned long COOLDOWN = 1000; // System must wait 1 seconds before counting again

int counter = 0; // stores amount of detected events
bool armed = false; // says if system is ready to count

void setup () { 
  Serial.begin(9600);
  HCSR04.begin(triggerPin, echoPin);
} // Opens Serial Monitor at 9600 baud and initializes the ultrasonic sensor pins

void sort(double a[], int n) { // Defines custom sorting function that sorts an array of doubles
  for (int i = 0; i < n - 1; ++i) { // Outer loop for the bubble sort
    for (int j = 0; j < n - 1 - i; ++j) { // Inner loop that compares neighboring values
      if (a[j] > a[j + 1]) { // Checks whether two neighboring values are out of order
        double temp = a[j];
        a[j] = a[j + 1];
        a[j + 1] = temp; // Swaps the two values
      }
    }
  }
}

void loop () {
  double* readingPtr = HCSR04.measureDistanceCm(); // Takes a distance measurement, library returns a pointer to a distance value
  double currentReading = *readingPtr; // Gets the actual distance value in centimeters

  if (currentReading > 0) { // Only accepts valid positive readings
    distances[indexer] = currentReading;
    indexer++;
  } // Stores the reading in the array, then moves to the next array position

  double sortBuffer[WINDOW_SIZE]; // Creates a temporary array for sorting
  for(int i = 0; i < WINDOW_SIZE; i++) {
    sortBuffer[i] = distances[i];
  } // Copies the stored distance readings into the temporary array

  sort(sortBuffer, WINDOW_SIZE); // Sorts the temporary array from smallest to largest
  double median = sortBuffer[WINDOW_SIZE / 2]; // Gets the middle value, this is the median value of 3
  // Example: [42, 300, 44] sorted into [42, 44, 300], median = 44
  // Helps reduce random bad readings
  
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
