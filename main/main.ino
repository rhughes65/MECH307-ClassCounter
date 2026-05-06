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

// void loop() {
//   encoderLoop();
//   // Example Usage:
//   startForward();
//   // delay(2048); // Spin forward for roughly one revolution
  
//   // stopMotor();
//   // delay(1000); // Wait a second
  
//   // startReverse();
//   // delay(2048); // Spin back
  
//   // stopMotor();
//   // delay(5000); // Wait 5 seconds
// }

// void loop() {
//   // // Move 2048 steps (1 full revolution) forward
//   // step(2048);
  
//   // // Unlike Stepper.h, this delay doesn't stop the motor. 
//   // // The motor is spinning IN THE BACKGROUND while we wait here.
//   // delay(10000); 
  
//   // // Move half a revolution backward
//   // step(-1024);
  
//   // delay(10000);

//   step(getEncCount()*10);
//   setEncCount(0);
//   delay(100);
// }
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
    Serial.println("Button B: Rotating -100 steps");
    step(-100);
    
    // Optional: Wait until button is released
    while(!ss.digitalRead(BUTTON_B)) delay(10);
  }

  tofLoop();

  // Small delay to keep the loop stable
  delay(10);
}