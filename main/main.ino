#include "Adafruit_seesaw.h"

// Initialize the Seesaw object
Adafruit_seesaw ss;

// Pin definitions for Seesaw Gamepad (Standard)
// Change these numbers if you are using a different Seesaw-based board
#define BUTTON_A  5
#define BUTTON_B  1

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
}

void loop() {
  // We check if the button is LOW because INPUT_PULLUP 
  // means the pin is HIGH by default and LOW when pressed.
  
  bool buttonAPressed = !ss.digitalRead(BUTTON_A);
  bool buttonBPressed = !ss.digitalRead(BUTTON_B);

  if (buttonAPressed) {
    Serial.println("Button A: Rotating 100 steps");
    step(100);
    
    // Optional: Wait until button is released to prevent "machine gun" stepping
    while(!ss.digitalRead(BUTTON_A)) delay(10); 
  }

  if (buttonBPressed) {
    Serial.println("Button B: Reset counter");
    resetCounter();
    
    // Wait until button is released
    while(!ss.digitalRead(BUTTON_B)) delay(10);
  }

  tofLoop();

  // Small delay to keep the loop stable
  delay(10);
}
