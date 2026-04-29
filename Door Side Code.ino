#include <SPI.h>
#include <nRF24L01.h> //part of the RF24 library
#include <RF24.h>

// Define NRF24L01 radio object with CE and CSN pins
RF24 radio(8, 9); // CE = 8, CSN = 9

// Define the pipe address (must be the same as the receiver)
// This does NOT need to be changed based on your team number.
const byte address[6] = "00001";

const int trigPin = 2;
const int echoPin = 3;
const int neg=-7;
const int pos=7;
int count=0;
long duration;
int distance[5];
int sum=0;
float distance2;
float distance1;
int difference=0;
int distanceint=0;
int newperson=0;
int starttime=0;
int average=0;
const int wait=500;
int rf=0;
int buzz=0;
int test=0;
// Structure to send joystick data
// This MUST match the struct in your controller code.
struct RF_Data {
  int count;
  int buzz;
  int test;
  //Put other variables in here that you want to pass via RF
};

void setup() {
  Serial.begin(9600);

  // Initialize joystick button pin
  pinMode(trigPin, OUTPUT); // Enable internal pull-up resistor. This allows us to skip adding our own pullup resistor in circuit.
  pinMode(echoPin, INPUT);

  // Initialize NRF24L01 module
  radio.begin();
  radio.setChannel(30);          // CHANGE THIS TO YOUR TEAM # * 2!!! Team 38 should use channel 76
  radio.setPALevel(RF24_PA_HIGH); // Power level (HIGH for better range), switch to low if experiencing soft resets
  radio.setDataRate(RF24_250KBPS); // Lower data rate = longer range
  radio.openWritingPipe(address); // Set transmitter address
  radio.stopListening();          // Set as transmitter
}

void loop() {
test++;

// Convert to cm
  distanceint = measure();
  
  if (distanceint<5 || distanceint > 150){
    newperson=1;
  }
  Serial.println(count);
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
  }
  if (difference>pos){
    count++;
    buzz++;
  }
  if (buzz==6){
    buzz=1;
  }
  }
  newperson=0;
  Serial.println();
  } 
  RF_Data data;

  // Read joystick values
  data.count = count;
  data.buzz = buzz;
  data.test=test;
  // Send data via NRF24L01
  bool success = radio.write(&data, sizeof(data));
  if (!success) {
  Serial.println("TX failed");
}
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
