#include <Stepper.h>

const int stepsPerRevolution = 2048;
const int stepsPerPosition = 204;   // adjust for your digit spacing

Stepper myStepper(stepsPerRevolution, 7, 5, 6, 4);

const int limitSwitchPin = 2;
const int motorSpeed = 10;

int currentPosition = 0;
bool homed = false;
int direction = 1;   // 1 = up, -1 = down

void setup() {
  Serial.begin(9600);
  myStepper.setSpeed(motorSpeed);
  pinMode(limitSwitchPin, INPUT_PULLUP);

  homeToZero();
}

void loop() {
  Serial.print("At position: ");
  Serial.println(currentPosition);
  delay(1000);

  int nextPosition = currentPosition + direction;

  // if we hit the top, reverse
  if (nextPosition > 9) {
    direction = -1;
    nextPosition = 8;
  }

  // if we hit the bottom, reverse
  if (nextPosition < 0) {
    direction = 1;
    nextPosition = 1;
  }

  moveToPosition(nextPosition);
}

void homeToZero() {
  Serial.println("Searching for 0...");

  while (digitalRead(limitSwitchPin) == HIGH) {
    myStepper.step(1);
    delay(3);
  }

  currentPosition = 0;
  homed = true;
  Serial.println("0 found.");

  // back off switch slightly
  while (digitalRead(limitSwitchPin) == LOW) {
    myStepper.step(-1);
    delay(3);
  }

  Serial.println("Ready to count.");
}

void moveToPosition(int targetPosition) {
  if (!homed) {
    Serial.println("Not homed yet.");
    return;
  }

  int stepDifference = targetPosition - currentPosition;
  int stepsToMove = abs(stepDifference * stepsPerPosition);
  int stepDir = (stepDifference > 0) ? 1 : -1;

  for (int i = 0; i < stepsToMove; i++) {
    // if switch is pressed, we are at 0
    if (digitalRead(limitSwitchPin) == LOW) {
      Serial.println("Switch hit. Reset to 0 and reversing.");
      currentPosition = 0;
      direction = 1;   // after hitting 0, always go upward

      // back off switch slightly
      while (digitalRead(limitSwitchPin) == LOW) {
        myStepper.step(-1);
        delay(3);
      }

      return;
    }

    myStepper.step(stepDir);
    delay(3);
  }

  currentPosition = targetPosition;
}
