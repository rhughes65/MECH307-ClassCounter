#include <Stepper.h>

const int stepsPerRevolution = 2048;
const int zeroOffsetSteps = 15;
const int motorSpeed = 10;

// Tens motor (pins 6,7,8,9 → reordered)
Stepper tensStepper(stepsPerRevolution, 6, 8, 7, 9);
const int tensSwitchPin = 2;

// Ones motor (pins 10,11,12,13 → reordered)
Stepper onesStepper(stepsPerRevolution, 10, 12, 11, 13);
const int onesSwitchPin = 3;

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
}

void loop() {
  // nothing here — just homing test
}

void homeMotor(Stepper &motor, int switchPin, String name) {
  Serial.print("Homing ");
  Serial.println(name);

  // Move forward until switch is pressed
  while (digitalRead(switchPin) == HIGH) {
    motor.step(1);
    delay(3);
  }

  Serial.print(name);
  Serial.println(" switch hit");

  // Back off until switch is released
  while (digitalRead(switchPin) == LOW) {
    motor.step(-1);
    delay(3);
  }

  // Move forward slightly to align zero visually
  for (int i = 0; i < zeroOffsetSteps; i++) {
    motor.step(1);
    delay(3);
  }

  Serial.print(name);
  Serial.println(" zero aligned");
}
