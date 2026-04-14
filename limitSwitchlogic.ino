#include <Stepper.h>

const int stepsPerRevolution = 1024;
const int stepsPerPosition = 204;   // adjust for your digit spacing

Stepper myStepper(stepsPerRevolution, 7, 6, 5, 4);

const int limitSwitchPin = 2;
const int motorSpeed = 10;   // slower speed for smoother visible motion

int currentPosition = 0;
bool homed = false;

void setup() {
  Serial.begin(9600);
  myStepper.setSpeed(motorSpeed);
  pinMode(limitSwitchPin, INPUT_PULLUP);

  homeToZero();
}

void loop() {
  // pause at 0 first
  Serial.print("At position: ");
  Serial.println(currentPosition);
  delay(1000);

  // count upward from 1 to 9, then back to 0
  for (int target = 1; target <= 9; target++) {
    moveToPosition(target);
    Serial.print("At position: ");
    Serial.println(currentPosition);
    delay(1000);
  }

  moveToPosition(0);
  Serial.print("At position: ");
  Serial.println(currentPosition);
  delay(1000);
}

void homeToZero() {
  Serial.println("Searching for 0...");

  // rotate until switch is pressed
  while (digitalRead(limitSwitchPin) == HIGH) {
    myStepper.step(1);
    delay(5);
  }

  // define this as position 0
  currentPosition = 0;
  homed = true;
  Serial.println("0 found.");

  // back off switch slightly so it is released
  while (digitalRead(limitSwitchPin) == LOW) {
    myStepper.step(-1);
    delay(5);
  }

  Serial.println("Ready to count from 0.");
}

void moveToPosition(int targetPosition) {
  if (!homed) {
    Serial.println("Not homed yet.");
    return;
  }

  // keep target in range 0-9
  targetPosition = targetPosition % 10;
  if (targetPosition < 0) {
    targetPosition += 10;
  }

  // odometer style: always move forward
  int stepDifference = targetPosition - currentPosition;
  if (stepDifference < 0) {
    stepDifference += 10;
  }

  int stepsToMove = stepDifference * stepsPerPosition;

  for (int i = 0; i < stepsToMove; i++) {
    // if switch is hit, that means we are at 0 again
    if (digitalRead(limitSwitchPin) == LOW) {
      currentPosition = 0;
      Serial.println("Switch hit, position reset to 0.");
      return;
    }

    myStepper.step(1);
    delay(5);
  }

  currentPosition = targetPosition;
}
