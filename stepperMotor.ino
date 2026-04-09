#include <LiquidCrystal.h>// Include lcd library
#include <Stepper.h>

// int for the ultrasonic
int mil=0;
int srt=0;
int distance[5];
int sum=0;
int count=0;
int difference=0;
int distanceint=0;
int newperson=0;
int starttime=0;
int average=0;
int rf=0;
int buzz=0;

// int for the stepper motor
int currentPosition = 0;
int targetPosition = 0;

// const int for LCD and buzzer
const int trigPin = 9;
const int echoPin = 10;
const int neg=-7;
const int pos=7;
const int buzzer=8;
const int wait=500;
const int rs = 24, en = 26, d4 = 28, d5 = 30, d6 = 32, d7 = 34;// Set up pins

const int stepsPerRevolution = 1024;   // 28BYJ-48 stepper
const int maxPosition = 10;            // positions 0 through 10
const int stepsPerPosition = 205;      // about 1024/10

volatile int button=2;

long duration;

float distance2;
float distance1;

Stepper myStepper(stepsPerRevolution, 7, 6, 5, 4);
LiquidCrystal lcd(rs,en,d4,d5,d6,d7);// Set up lcd function

void setup() {
  Serial.begin(9600);
  myStepper.setSpeed(15);
  Serial.println("Stepper test starting...");
  lcd.begin(16,2);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzer,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(button), reset, FALLING);
}

void moveToPosition(int targetPosition) {
  int stepDifference = targetPosition - currentPosition;
  int stepsToMove = stepDifference * stepsPerPosition;

  myStepper.step(stepsToMove);

  currentPosition = targetPosition;

  Serial.print("Moved to position: ");
  Serial.println(currentPosition);
}

void loop()
{
  lcd.setCursor(0, 0);
  lcd.print("MECH 307");
  lcd.setCursor(0, 1); // print the number of seconds since reset:
  lcd.print("Students: ");
  lcd.print(count);
 

  // Convert to cm
  distanceint = measure();
 
 
  if (distanceint<5 || distanceint > 150){
    newperson=1;
  }
  Serial.println(distanceint);
  if (newperson==1){
  starttime=millis();
  distance1 = measure();
  for (rf=millis()-starttime; rf<=wait+100;){
    if ((millis()-starttime)>wait) {
    distance2 = measure();
    }
    rf=millis()-starttime;
  }
 
  difference=distance2-distance1;
  //Serial.print(distance1);
  if (difference < 100 && distance2>0){
  if (difference<neg && count>=1){
    count--;
    buzz--;
    targetPosition--;
  }
  if (difference>pos){
    count++;
    buzz++;
    targetPosition++;
  }
  if (buzz==5){
    tone(buzzer, 500, 500);
    buzz=0;
  }
  }
  newperson=0;
  Serial.println();
  }
      // Move from 0 up to 10
    moveToPosition(targetPosition);
}

int measure(){
  sum=0;
  average=0;
  for (int i=0; i<5 ; i++){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read echo
  duration = pulseIn(echoPin, HIGH, 30000);

  // Convert to cm
  distance[i] = duration * 0.034 / 2;
  }
  for (int j=0; j<5; j++){
    sum +=distance[j];
  }
  average=sum/2.5;
  return average;

}

void reset(){
  lcd.clear();
  count=0;
  buzz=0;
}
