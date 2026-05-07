#include "Adafruit_seesaw.h"

// Configuration
#define NUM_TOF_SENSORS 3

// Global variables used by multiple modules
int buzz = 0;

// Initialize the Seesaw object
Adafruit_seesaw ss;

// Pin definitions for Seesaw Gamepad (Standard)
// Change these numbers if you are using a different Seesaw-based board
#define BUTTON_A 5
#define BUTTON_B 1
#define BUTTON_Y 2
#define BUTTON_X 6

void setup() {
  Serial.begin(115200);


  motorSetup();
  // encoderSetup();
  tofSetup();
  counterSetup();

  // Initialize Seesaw
  if (!ss.begin(0x50)) {
    Serial.println("Seesaw not found!");
    while(1) delay(10);
  }
  Serial.println("Seesaw started!");

  // Set up button pins with internal pullups
  ss.pinMode(BUTTON_A, INPUT_PULLUP);
  ss.pinMode(BUTTON_B, INPUT_PULLUP);
  ss.pinMode(BUTTON_X, INPUT_PULLUP);
  ss.pinMode(BUTTON_Y, INPUT_PULLUP);
}

void loop() {
  // We check if the button is LOW because INPUT_PULLUP 
  // means the pin is HIGH by default and LOW when pressed.
  
  bool buttonAPressed = !ss.digitalRead(BUTTON_A);
  bool buttonBPressed = !ss.digitalRead(BUTTON_B);
  bool buttonXPressed = !ss.digitalRead(BUTTON_X);
  bool buttonYPressed = !ss.digitalRead(BUTTON_Y);

  if (buttonAPressed) {
    Serial.println("Button A: Rotating 100 steps");
    step(100);
    
    // Optional: Wait until button is released to prevent "machine gun" stepping
    while(!ss.digitalRead(BUTTON_A)) delay(10); 
  }

  if (buttonBPressed) {
    Serial.println("Button B: Rotating back 100 steps");
    step(-100);
    
    // Wait until button is released
    while(!ss.digitalRead(BUTTON_B)) delay(10);
  }

  if (buttonXPressed){
    Serial.println("Button X: Reset Counter");
    resetCounter();

    while(!ss.digitalRead(BUTTON_X)) delay(10);
  }

  tofLoop();

  updateCounter(getD1(), getD2(), getD3());


  // Small delay to keep the loop stable
  // delay(10);
}
