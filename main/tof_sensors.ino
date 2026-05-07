#include "Adafruit_VL53L0X.h"

#ifndef NUM_TOF_SENSORS
#define NUM_TOF_SENSORS 3
#endif

// Unique I2C addresses
#define LOX1_ADDRESS 0x30
#define LOX2_ADDRESS 0x31
#define LOX3_ADDRESS 0x32

// Your specified XSHUT pins
#define SHT_LOX1 22
#define SHT_LOX2 23
#define SHT_LOX3 24

// Sensor objects
#if NUM_TOF_SENSORS >= 1
Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
#endif
#if NUM_TOF_SENSORS >= 2
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();
#endif
#if NUM_TOF_SENSORS >= 3
Adafruit_VL53L0X lox3 = Adafruit_VL53L0X();
#endif

void tofSetup() {
  // 1. Initialize all possible XSHUT pins as OUTPUT and LOW
  // This ensures unused sensors stay powered off
  pinMode(SHT_LOX1, OUTPUT);
  digitalWrite(SHT_LOX1, LOW);
  pinMode(SHT_LOX2, OUTPUT);
  digitalWrite(SHT_LOX2, LOW);
  pinMode(SHT_LOX3, OUTPUT);
  digitalWrite(SHT_LOX3, LOW);
  
  delay(10);

  // 2. Initialize active sensors one by one
  
  #if NUM_TOF_SENSORS >= 1
  digitalWrite(SHT_LOX1, HIGH);
  delay(10);
  if(!lox1.begin(LOX1_ADDRESS)) {
    Serial.println(F("Failed to boot first VL53L0X"));
    while(1);
  }
  #endif

  #if NUM_TOF_SENSORS >= 2
  digitalWrite(SHT_LOX2, HIGH);
  delay(10);
  if(!lox2.begin(LOX2_ADDRESS)) {
    Serial.println(F("Failed to boot second VL53L0X"));
    while(1);
  }
  #endif

  #if NUM_TOF_SENSORS >= 3
  digitalWrite(SHT_LOX3, HIGH);
  delay(10);
  if(!lox3.begin(LOX3_ADDRESS)) {
    Serial.println(F("Failed to boot third VL53L0X"));
    while(1);
  }
  #endif
}

int d1 = 0;
int d2 = 0;
int d3 = 0;
int getD1() { return d1; }
int getD2() { return d2; }
int getD3() { return d3; }

void tofLoop() {
  VL53L0X_RangingMeasurementData_t measure;

  #if NUM_TOF_SENSORS >= 1
  lox1.rangingTest(&measure, false);
  // Status 4 is out of range. 8190+ is also usually out of range.
  // Map these to a stable high value (2000mm) instead of 0.
  if (measure.RangeStatus == 4 || measure.RangeMilliMeter > 2000) d1 = 2000;
  else d1 = measure.RangeMilliMeter;
  #else
  d1 = 2000;
  #endif

  #if NUM_TOF_SENSORS >= 2
  lox2.rangingTest(&measure, false);
  if (measure.RangeStatus == 4 || measure.RangeMilliMeter > 2000) d2 = 2000;
  else d2 = measure.RangeMilliMeter;
  #else
  d2 = 2000;
  #endif

  #if NUM_TOF_SENSORS >= 3
  lox3.rangingTest(&measure, false);
  if (measure.RangeStatus == 4 || measure.RangeMilliMeter > 2000) d3 = 2000;
  else d3 = measure.RangeMilliMeter;
  #else
  d3 = 2000;
  #endif

  logTOF();
}

void logTOF() {
  // Format for Arduino Serial Plotter: Label1:Value1,Label2:Value2,...
  #if NUM_TOF_SENSORS >= 1
  Serial.print(F("S1:"));
  Serial.print(d1);
  #endif
  
  #if NUM_TOF_SENSORS >= 2
  Serial.print(F(",S2:"));
  Serial.print(d2);
  #endif
  
  #if NUM_TOF_SENSORS >= 3
  Serial.print(F(",S3:"));
  Serial.print(d3);
  #endif

  Serial.println();
}
