#include <Stepper.h>

const int stepsPerRevolution = 1024;
const int stepsPerPosition = 205;

Stepper myStepper(stepsPerRevolution, 7, 6, 5, 4);

const int limitSwitchPin = 2;
const int motorSpeed = 15;

int currentPosition = 0;

void setup() {
  Serial.begin(9600);
  myStepper.setSpeed(motorSpeed);
  pinMode(limitSwitchPin, INPUT_PULLUP);
}

void moveToPosition(int targetPosition) {
  int stepDifference = targetPosition - currentPosition;
  int stepsToMove = stepDifference * stepsPerPosition;

  int stepDirection = 1;
  if (stepsToMove < 0) {
    stepDirection = -1;
    stepsToMove = -stepsToMove;
  }

  for (int i = 0; i < stepsToMove; i++) {
    if (digitalRead(limitSwitchPin) == LOW) {
      Serial.println("Limit switch hit. Motion stopped.");
      return;
    }

    myStepper.step(stepDirection);
    delay(5);
  }

  currentPosition = targetPosition;
  Serial.print("Moved to position: ");
  Serial.println(currentPosition);
}

void loop() {
  moveToPosition(5);
  delay(1000);
  moveToPosition(0);
  delay(1000);
}
