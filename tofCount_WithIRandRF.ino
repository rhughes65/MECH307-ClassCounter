#include "Adafruit_VL53L0X.h"
#include <SPI.h>
#include <nRF24L01.h> //part of the RF24 library
#include <RF24.h>
#include "IRremote.h"

RF24 radio(5, 4); // CE = 8, CSN = 9
// Define the pipe address (must be the same as the receiver)
const byte address[6] = "00001";
int buzz = 0;



Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// --- Configuration ---
const int DETECTION_THRESHOLD = 1000; // Distance in mm (e.g., 1 meter)
const int DEBOUNCE_DELAY = 50;        // Small delay to stabilize readings

int personCount = 0;
bool isPersonDetected = false;

struct RF_Data {
  int personCount;
  int buzz;
  //Put other variables in here that you want to pass via RF
};

int receiver = 2;
volatile int interrupted=0;

IRrecv irrecv(receiver);
decode_results results;

void setup() {
  Serial.begin(115200);

  while (!Serial) {
    delay(1);
  }
  
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
  
  Serial.println(F("Person Counter Initialized..."));
  Serial.print("Current Count: ");
  Serial.println(personCount);
  radio.begin();
  radio.setChannel(30);          
  radio.setPALevel(RF24_PA_HIGH); // Power level (HIGH for better range), switch to low if experiencing soft resets
  radio.setDataRate(RF24_250KBPS); // Lower data rate = longer range
  radio.openWritingPipe(address); // Set transmitter address
  radio.stopListening();          // Set as transmitter
  IrReceiver.begin(receiver, ENABLE_LED_FEEDBACK); // Start the receiver
}

void loop() {

  if (IrReceiver.decode()) {
  uint32_t code = IrReceiver.decodedIRData.decodedRawData;
  Serial.print("IR received: 0x");
  Serial.println(code, HEX);
  
  if (code == 0xE916FF00) reset();
  
  IrReceiver.resume();
}

  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false); 

  // Check if the reading is valid (RangeStatus 0 is a clean hit)
  if (measure.RangeStatus != 4) {
    int currentDistance = measure.RangeMilliMeter;

    // Logic: If distance is less than threshold and we weren't already detecting someone
    if (currentDistance < DETECTION_THRESHOLD && !isPersonDetected) {
      isPersonDetected = true;
      personCount++;
      buzz++;
      Serial.print("Person Detected! Total Count: ");
      Serial.println(personCount);
    } 
    if (buzz==6){
    buzz=1;
  }
    // Logic: If distance returns to normal, reset the detection state
    else if (currentDistance >= DETECTION_THRESHOLD && isPersonDetected) {
      isPersonDetected = false;
      Serial.println("Path Clear.");
    }
    RF_Data data;

  // Read joystick values
  data.personCount = personCount;
  data.buzz = buzz;
  // Send data via NRF24L01
  bool success = radio.write(&data, sizeof(data));
  if (!success) {
  Serial.println("TX failed");
  }

  delay(DEBOUNCE_DELAY);
  }
}

void reset(){
  personCount=0;
  buzz=0;
}
