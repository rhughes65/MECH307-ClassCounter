#include <Stepper.h>
#include <EEPROM.h>

const int stepsPerRevolution = 2048;
const int eepromAddress = 0; // Starting memory address

Stepper myStepper(stepsPerRevolution, 4, 6, 5, 7);

int currentPosition = 0;

void setup() {
  Serial.begin(9600);
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

void loop() {
  // Move 1 step at a time to track progress
  for (int i = 0; i < stepsPerRevolution; i+=10) {
    myStepper.step(10);
    currentPosition+=10;

    // 3. Save to EEPROM every 10th step
    EEPROM.put(eepromAddress, currentPosition);
    Serial.println(currentPosition);
  }


}
