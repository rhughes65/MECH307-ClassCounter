#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Stepper.h>

// ================= NRF24L01 =================
// Moved because pins 8 and 9 are used by tens motor
RF24 radio(A0, A1); // CE = A0, CSN = A1
const byte address[6] = "00001";

// ================= ULTRASONIC =================
// Moved because pins 2 and 3 are used by limit switches
const int trigPin = A2;
const int echoPin = A3;

// ================= STEPPERS =================
const int stepsPerRevolution = 2048;
const int stepsPerPosition = 205;
const int zeroOffsetSteps = 15;

const int motorSpeed = 10;
const int homingSpeed = 5;
const int homingDelay = 8;

// Tens motor pins: 6, 7, 8, 9
Stepper tensStepper(stepsPerRevolution, 6, 8, 7, 9);
const int tensSwitchPin = 2;

// Ones motor pins: 10, 11, 12, 13
Stepper onesStepper(stepsPerRevolution, 10, 12, 11, 13);
const int onesSwitchPin = 3;

// Current displayed number
int currentNumber = 0;
int tens = 0;
int ones = 0;

// ================= COUNTING VARIABLES =================
const int neg = -7;
const int pos = 7;

int count = 0;
long duration;
int distance[5];
int sum = 0;
float distance2;
float distance1;
int difference = 0;
int distanceint = 0;
int newperson = 0;
unsigned long starttime = 0;
int average = 0;
const int wait = 500;
int rf = 0;
int buzz = 0;
int test = 0;

// ================= RF DATA STRUCT =================
struct RF_Data {
  int count;
  int buzz;
  int test;
};

void setup() {
  Serial.begin(9600);

  // Ultrasonic setup
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Stepper setup
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

  // RF setup
  radio.begin();
  radio.setChannel(30);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(address);
  radio.stopListening();

  Serial.println("System ready.");
}

void loop() {
  test++;

  distanceint = measure();

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
      if (difference < neg && count >= 1) {
        count--;
        buzz--;
      }

      if (difference > pos) {
        count++;
        buzz++;
      }

      if (buzz == 6) {
        buzz = 1;
      }

      if (count < 0) {
        count = 0;
      }

      if (count > 99) {
        count = 99;
      }

      moveToNumber(count);
    }

    newperson = 0;
  }

  RF_Data data;
  data.count = count;
  data.buzz = buzz;
  data.test = test;

  bool success = radio.write(&data, sizeof(data));

  if (!success) {
    Serial.println("TX failed");
  }

  Serial.print("Count: ");
  Serial.println(count);
}

// ================= STEPPER COUNTING =================

void moveToNumber(int targetNumber) {
  if (targetNumber == currentNumber) {
    return;
  }

  if (targetNumber > currentNumber) {
    while (currentNumber < targetNumber) {
      countUpOne();
      currentNumber++;
      delay(250);
    }
  } else {
    while (currentNumber > targetNumber) {
      countDownOne();
      currentNumber--;
      delay(250);
    }
  }
}

void countUpOne() {
  if (ones < 9) {
    moveOneDigit(onesStepper, onesSwitchPin, 1, "ONES");
    ones++;
  } else {
    moveOneDigit(onesStepper, onesSwitchPin, 1, "ONES");
    ones = 0;

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
    moveOneDigit(onesStepper, onesSwitchPin, -1, "ONES");
    ones = 9;

    if (tens > 0) {
      moveOneDigit(tensStepper, tensSwitchPin, -1, "TENS");
      tens--;
    }
  }
}

void moveOneDigit(Stepper &motor, int switchPin, int dir, String name) {
  for (int i = 0; i < stepsPerPosition; i++) {
    if (digitalRead(switchPin) == LOW) {
      Serial.print(name);
      Serial.println(" switch hit unexpectedly. Re-homing...");
      homeMotor(motor, switchPin, name);
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
