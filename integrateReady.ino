#include <Stepper.h>

const int stepsPerRevolution = 2048;
const int stepsPerPosition = 205;
const int zeroOffsetSteps = 15;

// Speeds
const int motorSpeed = 10;
const int homingSpeed = 5;
const int homingDelay = 8;

// Tens motor
Stepper tensStepper(stepsPerRevolution, 6, 8, 7, 9);
const int tensSwitchPin = 2;

// Ones motor
Stepper onesStepper(stepsPerRevolution, 10, 12, 11, 13);
const int onesSwitchPin = 3;

// Current display value
int currentNumber = 0;
int tens = 0;
int ones = 0;

void setup() {
  Serial.begin(9600);

  tensStepper.setSpeed(motorSpeed);
  onesStepper.setSpeed(motorSpeed);

  pinMode(tensSwitchPin, INPUT_PULLUP);
  pinMode(onesSwitchPin, INPUT_PULLUP);

  Serial.println("Starting homing...");

  homeMotor(tensStepper, tensSwitchPin, "TENS");
  homeMotor(onesStepper, onesSwitchPin, "ONES");

  currentNumber = 0;
  tens = 0;
  ones = 0;

  Serial.println("Homing complete.");
  Serial.println("Enter a number from 0 to 99:");
}

void loop() {
  if (Serial.available() > 0) {
    int targetNumber = Serial.parseInt();

    while (Serial.available() > 0) {
      Serial.read();
    }

    if (targetNumber >= 0 && targetNumber <= 99) {
      Serial.print("Target received: ");
      Serial.println(targetNumber);

      moveToNumber(targetNumber);

      Serial.print("Now displaying: ");
      if (currentNumber < 10) {
        Serial.print("0");
      }
      Serial.println(currentNumber);

      Serial.println("Enter another number from 0 to 99:");
    } else {
      Serial.println("Invalid input. Enter a number from 0 to 99:");
    }
  }
}

// ================= COUNTING =================

void moveToNumber(int targetNumber) {
  if (targetNumber == currentNumber) {
    Serial.println("Already at that number.");
    return;
  }

  if (targetNumber > currentNumber) {
    Serial.println("Counting UP");

    while (currentNumber < targetNumber) {
      countUpOne();
      currentNumber++;

      Serial.print("Display: ");
      if (currentNumber < 10) {
        Serial.print("0");
      }
      Serial.println(currentNumber);

      delay(500);
    }
  } else {
    Serial.println("Counting DOWN");

    while (currentNumber > targetNumber) {
      countDownOne();
      currentNumber--;

      Serial.print("Display: ");
      if (currentNumber < 10) {
        Serial.print("0");
      }
      Serial.println(currentNumber);

      delay(500);
    }
  }
}

void countUpOne() {
  if (ones < 9) {
    moveOneDigit(onesStepper, onesSwitchPin, 1, "ONES");
    ones++;
  } else {
    // ones rolls from 9 to 0
    moveOneDigit(onesStepper, onesSwitchPin, 1, "ONES");
    ones = 0;

    // tens increases by 1
    if (tens < 9) {
      moveOneDigit(tensStepper, tensSwitchPin, 1, "TENS");
      tens++;
    }
  }
}

void countDownOne() {
  if (ones > 0) {
    moveOneDigit(onesStepper, onesSwitchPin, -1, "ONES");
    ones--;
  } else {
    // ones rolls from 0 to 9
    moveOneDigit(onesStepper, onesSwitchPin, -1, "ONES");
    ones = 9;

    // tens decreases by 1
    if (tens > 0) {
      moveOneDigit(tensStepper, tensSwitchPin, -1, "TENS");
      tens--;
    }
  }
}

// ================= MOTOR STEP =================

void moveOneDigit(Stepper &motor, int switchPin, int dir, String name) {
  for (int i = 0; i < stepsPerPosition; i++) {
    if (digitalRead(switchPin) == LOW) {
      Serial.print(name);
      Serial.println(" switch hit unexpectedly. Re-homing...");

      homeMotor(motor, switchPin, name);

      if (name == "ONES") {
        ones = 0;
      } else if (name == "TENS") {
        tens = 0;
      }

      currentNumber = tens * 10 + ones;
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

  while (digitalRead(switchPin) == HIGH) {
    motor.step(1);
    delay(homingDelay);
  }

  Serial.print(name);
  Serial.println(" switch hit");

  while (digitalRead(switchPin) == LOW) {
    motor.step(-1);
    delay(homingDelay);
  }

  for (int i = 0; i < zeroOffsetSteps; i++) {
    motor.step(1);
    delay(homingDelay);
  }

  Serial.print(name);
  Serial.println(" zero aligned");

  motor.setSpeed(motorSpeed);
}
