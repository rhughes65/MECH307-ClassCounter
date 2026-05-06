#include "Adafruit_VL53L0X.h"

// Unique I2C addresses
#define LOX1_ADDRESS 0x30
#define LOX2_ADDRESS 0x31
#define LOX3_ADDRESS 0x32
// #define LOX4_ADDRESS 0x33

// Your specified XSHUT pins
#define SHT_LOX1 22
#define SHT_LOX2 23
#define SHT_LOX3 24
// #define SHT_LOX4 25

// Sensor objects
Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox3 = Adafruit_VL53L0X();
// Adafruit_VL53L0X lox4 = Adafruit_VL53L0X();

void setID() {
  // Reset all
  digitalWrite(SHT_LOX1, LOW);    
  digitalWrite(SHT_LOX2, LOW);
  digitalWrite(SHT_LOX3, LOW);
  // digitalWrite(SHT_LOX4, LOW);
  delay(10);
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, HIGH);
  digitalWrite(SHT_LOX3, HIGH);
  // digitalWrite(SHT_LOX4, HIGH);
  delay(10);

  // Initialize Sensor 1
  digitalWrite(SHT_LOX2, LOW);
  digitalWrite(SHT_LOX3, LOW);
  // digitalWrite(SHT_LOX4, LOW);
  if(!lox1.begin(LOX1_ADDRESS)) while(1);

  // Initialize Sensor 2
  digitalWrite(SHT_LOX2, HIGH);
  delay(10);
  if(!lox2.begin(LOX2_ADDRESS)) while(1);

  // Initialize Sensor 3
  digitalWrite(SHT_LOX3, HIGH);
  delay(10);
  if(!lox3.begin(LOX3_ADDRESS)) while(1);

  // // Initialize Sensor 4
  // digitalWrite(SHT_LOX4, HIGH);
  // delay(10);
  // if(!lox4.begin(LOX4_ADDRESS)) while(1);
}

void tofSetup() {
  Serial.begin(115200);
  while (!Serial) delay(1);

  pinMode(SHT_LOX1, OUTPUT);
  pinMode(SHT_LOX2, OUTPUT);
  pinMode(SHT_LOX3, OUTPUT);
  // pinMode(SHT_LOX4, OUTPUT);

  setID();

  // Header format per your example
  Serial.println(F("# Optional header defines series names"));
  Serial.println(F("# time(ms), sensor1(mm), sensor2(mm), sensor3(mm), sensor4(mm)"));
}

void tofLoop() {
  VL53L0X_RangingMeasurementData_t m1, m2, m3;
  
  lox1.rangingTest(&m1, false);
  lox2.rangingTest(&m2, false);
  lox3.rangingTest(&m3, false);
  // lox4.rangingTest(&m4, false);

  // Extract distance values
  int d1 = (m1.RangeStatus != 4) ? m1.RangeMilliMeter : 0;
  int d2 = (m2.RangeStatus != 4) ? m2.RangeMilliMeter : 0;
  int d3 = (m3.RangeStatus != 4) ? m3.RangeMilliMeter : 0;

  // Data format: time, val1, val2, val3, val4
  Serial.print(millis());
  Serial.print(F(", "));
  
  Serial.print(d1);
  Serial.print(F(", "));
  Serial.print(d2);
  Serial.print(F(", "));
  Serial.print(d3);
  // Serial.print(F(", "));
  // Serial.print((m4.RangeStatus != 4) ? m4.RangeMilliMeter : 0);

  Serial.println("");

  // Update people counter with sensor data
  updateCounter(d1, d2, d3);

  delay(100);
}
