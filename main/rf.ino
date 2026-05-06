#include <SPI.h>
#include <nRF24L01.h> //part of the RF24 library
#include <RF24.h>
RF24 radio(5, 4); // CE = 5, CSN = 4
// Define the pipe address (must be the same as the receiver)
const byte address[6] = "00001";

struct RF_Data {
  int count;
  int buzz;
  //Put other variables in here that you want to pass via RF
};

void RFsetup() {
  radio.begin();
  radio.setChannel(30);          
  radio.setPALevel(RF24_PA_HIGH); // Power level (HIGH for better range), switch to low if experiencing soft resets
  radio.setDataRate(RF24_250KBPS); // Lower data rate = longer range
  radio.openWritingPipe(address); // Set transmitter address
  radio.stopListening();          // Set as transmitter
}

void RFloop() {
  RF_Data data;
  // Read joystick values
  data.count = count;
  data.buzz = buzz;
  // Send data via NRF24L01
  bool success = radio.write(&data, sizeof(data));
  if (!success) {
  Serial.println("TX failed");
}
}