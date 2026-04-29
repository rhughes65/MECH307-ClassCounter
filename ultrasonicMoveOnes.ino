#include <Stepper.h>

// ================= ULTRASONIC =================
const int trigPin = 48;
const int echoPin = 49;

// ================= ONES STEPPER ONLY =================
const int stepsPerRevolution = 2048;
const int stepsPerPosition = 205;
const int zeroOffsetSteps = 15;

const int motorSpeed = 10;
const int homingSpeed = 5;
const int homingDelay = 8;

// Ones motor pins: 10, 11, 12, 13
Stepper onesStepper(stepsPerRevolution, 10, 12, 11, 13);
const int onesSwitchPin = 3;

int ones = 0;
int currentNumber = 0;

// ================= COUNTING VARIABLES =================
const int neg = -7;
const int pos = 7;

int count = 0;
long duration;
int distance[5];
int sum = 0;
float distance2 = 0;
float distance1 = 0;
int difference = 0;
int distanceint = 0;
int newperson = 0;
unsigned long starttime = 0;
int average = 0;
const int wait = 500;

void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  onesStepper.setSpeed(motorSpeed);
  pinMode(onesSwitchPin, INPUT_PULLUP);

  Serial.println("Starting ONES homing...");
  homeMotor(onesStepper, onesSwitchPin, "ONES");

  ones = 0;
  currentNumber = 0;

  Serial.println("ONES homing complete.");
  Serial.println("Debugging ones digit only, range 0 to 9.");
}

void loop() {
  distanceint = measure();
  Serial.println(distanceint);
  if (distanceint < 5 || distanceint > 150) {
    newperson = 1;
  }

  if (newperson == 1) {
    starttime = millis();
    distance1 = measure();

    while ((millis() - starttime) <= wait + 100) {
      if ((millis() - starttime) > wait) {
        distance2 = measure();
      }
    }

    difference = distance2 - distance1;

    if (difference < 100 && distance2 > 0) {
      int previousCount = count;

      if (difference < neg && count >= 1) {
        count--;
      }

      if (difference > pos && count < 9) {
        count++;
      }

      count = constrain(count, 0, 9);

      if (count != previousCount) {
        moveToNumber(count);
      }
    }

    newperson = 0;
  }

  Serial.print("Count: ");
  Serial.println(count);

  delay(100);
}

// ================= ONES COUNTING =================

void moveToNumber(int targetNumber) {
  if (targetNumber == currentNumber) {
    return;
  }

  if (targetNumber > currentNumber) {
    while (currentNumber < targetNumber) {
      moveOneDigit(onesStepper, onesSwitchPin, 1, "ONES");
      currentNumber++;
      ones = currentNumber;

      Serial.print("ONES Display: ");
      Serial.println(ones);

      delay(250);
    }
  } else {
    while (currentNumber > targetNumber) {
      moveOneDigit(onesStepper, onesSwitchPin, -1, "ONES");
      currentNumber--;
      ones = currentNumber;

      Serial.print("ONES Display: ");
      Serial.println(ones);

      delay(250);
    }
  }
}

void moveOneDigit(Stepper &motor, int switchPin, int dir, String name) {
  for (int i = 0; i < stepsPerPosition; i++) {
    if (digitalRead(switchPin) == LOW) {
      Serial.print(name);
      Serial.println(" switch hit unexpectedly. Re-homing...");

      homeMotor(motor, switchPin, name);

      ones = 0;
      currentNumber = 0;
      count = 0;

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

// ================= ULTRASONIC MEASURE =================

int measure() {
  sum = 0;
  average = 0;

  for (int i = 0; i < 5; i++) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH, 30000);
    distance[i] = duration * 0.034 / 2;
  }

  for (int j = 0; j < 5; j++) {
    sum += distance[j];
  }

  average = sum / 2.5;
  return average;
}
