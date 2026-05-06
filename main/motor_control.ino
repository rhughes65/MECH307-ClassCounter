#include <Stepper.h>
#include <EEPROM.h>

#define MOTOR_STEPS_TO_SAVE 10

const int stepsPerRevolution = 2048;
const int eepromAddress = 0; // Starting memory address

Stepper myStepper(stepsPerRevolution, 4, 6, 5, 7);

int currentPosition = 0;

void motorSetup() {
  myStepper.setSpeed(12);

  // 1. Retrieve the last saved position from EEPROM
  EEPROM.get(eepromAddress, currentPosition);

  // 2. If we weren't at zero when power was lost, go back to zero
  if (currentPosition != 0) {
    Serial.print("Recovering from power loss. Returning ");
    Serial.print(currentPosition);
    Serial.println(" steps to zero.");
    
    // Step negatively to return to home
    myStepper.step(-currentPosition);
    
    // Reset position and update EEPROM to zero
    currentPosition = 0;
    EEPROM.put(eepromAddress, currentPosition);
  }

  Serial.println("System Ready. Beginning rotation...");
}

void motorLoop() {
  myStepper.step(MOTOR_STEPS_TO_SAVE);
  currentPosition+=MOTOR_STEPS_TO_SAVE;
  EEPROM.put(eepromAddress, currentPosition);
}