#include <Stepper.h>

const int stepsPerRevolution = 2048;
const int stepsPerPosition = 205;   // adjust if needed
const int zeroOffsetSteps = 15;     // adjust to align displayed 0

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

  Serial.println("Homing complete.");
  Serial.println("Enter a number from 0 to 9.");
}

void loop() {
  if (Serial.available() > 0) {
    int requestedNumber = Serial.parseInt();

    while (Serial.available() > 0) {
      Serial.read();
    }

    if (requestedNumber >= 0 && requestedNumber <= 9) {
      Serial.print("Received target: ");
      Serial.println(requestedNumber);

      moveToPosition(requestedNumber);

      Serial.print("Now at position: ");
      Serial.println(currentPosition);
      Serial.println("Enter another number from 0 to 9.");
    } else {
      Serial.println("Invalid input. Enter a number from 0 to 9.");
    }
  }
}

void homeToZero() {
  Serial.println("Searching for 0...");

  // Move until switch is pressed
  while (digitalRead(limitSwitchPin) == HIGH) {
    myStepper.step(1);
    delay(3);
  }

  Serial.println("Switch hit.");

  // Back off until switch is released
  while (digitalRead(limitSwitchPin) == LOW) {
    myStepper.step(-1);
    delay(3);
  }

  // Move forward slightly to align visual 0
  for (int i = 0; i < zeroOffsetSteps; i++) {
    myStepper.step(1);
    delay(3);
  }

  currentPosition = 0;
  homed = true;
  direction = 1;

  Serial.println("Zero aligned.");
}

void moveToPosition(int targetPosition) {
  if (!homed) {
    Serial.println("Not homed yet.");
    return;
  }

  if (targetPosition == currentPosition) {
    Serial.println("Already at that position.");
    return;
  }

  // Decide direction based on next requested number
  if (targetPosition > currentPosition) {
    direction = 1;
  } else {
    direction = -1;
  }

  Serial.print("Direction: ");
  if (direction == 1) {
    Serial.println("UP");
  } else {
    Serial.println("DOWN");
  }

  // Count step-by-step through positions
  while (currentPosition != targetPosition) {
    int nextPosition = currentPosition + direction;

    moveOnePosition(direction);

    // If homing occurred during motion, stop
    if (!homed) {
      return;
    }

    currentPosition = nextPosition;

    Serial.print("At position: ");
    Serial.println(currentPosition);

    delay(500);
  }
}

void moveOnePosition(int stepDirection) {
  for (int i = 0; i < stepsPerPosition; i++) {
    // If switch is hit, re-home to 0
    if (digitalRead(limitSwitchPin) == LOW) {
      Serial.println("Switch hit during motion. Re-homing...");
      homed = false;
      homeToZero();
      return;
    }

    myStepper.step(stepDirection);
    delay(3);
  }
}
