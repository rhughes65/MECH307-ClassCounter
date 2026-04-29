#include <Stepper.h>

const int stepsPerRevolution = 2048;
const int stepsPerPosition = 205;
const int zeroOffsetSteps = 15;

// Speeds
const int motorSpeed = 10;    // normal operation
const int homingSpeed = 5;    // slower homing
const int homingDelay = 8;    // extra delay for smooth homing

// Tens motor (pins 6,7,8,9 → reordered)
Stepper tensStepper(stepsPerRevolution, 6, 8, 7, 9);
const int tensSwitchPin = 2;

// Ones motor (pins 10,11,12,13 → reordered)
Stepper onesStepper(stepsPerRevolution, 10, 12, 11, 13);
const int onesSwitchPin = 3;

// Positions
int ones = 0;
int tens = 0;

void setup() {
  Serial.begin(9600);

  tensStepper.setSpeed(motorSpeed);
  onesStepper.setSpeed(motorSpeed);

  pinMode(tensSwitchPin, INPUT_PULLUP);
  pinMode(onesSwitchPin, INPUT_PULLUP);

  Serial.println("Starting homing...");

  homeMotor(tensStepper, tensSwitchPin, "TENS");
  homeMotor(onesStepper, onesSwitchPin, "ONES");

  Serial.println("Homing complete.");

  delay(1000);
}

void loop() {
  countUpToTen();
  while (true); // stop after one run
}

// ================= COUNTING =================

void countUpToTen() {
  for (int i = 1; i <= 10; i++) {
    Serial.print("Counting to: ");
    Serial.println(i);

    incrementNumber();

    Serial.print("Display: ");
    Serial.print(tens);
    Serial.println(ones);

    delay(1000);
  }
}

void incrementNumber() {
  if (ones < 9) {
    moveOneDigit(onesStepper, onesSwitchPin, 1);
    ones++;
  } else {
    // rollover 9 → 0
    moveOneDigit(onesStepper, onesSwitchPin, 1);
    ones = 0;

    // increment tens
    moveOneDigit(tensStepper, tensSwitchPin, 1);
    tens++;
  }
}

// ================= MOTOR STEP =================

void moveOneDigit(Stepper &motor, int switchPin, int dir) {
  for (int i = 0; i < stepsPerPosition; i++) {
    if (digitalRead(switchPin) == LOW) {
      Serial.println("Switch hit unexpectedly. Re-homing...");
      homeMotor(motor, switchPin, "REHOME");
      return;
    }

    motor.step(dir);
    delay(3);
  }
}

// ================= HOMING =================

void homeMotor(Stepper &motor, int switchPin, String name) {
  Serial.print("Homing ");
  Serial.println(name);

  motor.setSpeed(homingSpeed);

  // Move toward switch
  while (digitalRead(switchPin) == HIGH) {
    motor.step(1);
    delay(homingDelay);
  }

  Serial.print(name);
  Serial.println(" switch hit");

  // Back off
  while (digitalRead(switchPin) == LOW) {
    motor.step(-1);
    delay(homingDelay);
  }

  // Final alignment
  for (int i = 0; i < zeroOffsetSteps; i++) {
    motor.step(1);
    delay(homingDelay);
  }

  Serial.print(name);
  Serial.println(" zero aligned");

  motor.setSpeed(motorSpeed); // restore normal speed
}
