#include "IRremote.h"

int receiver = 2;
volatile int interrupted=0;

IRrecv irrecv(receiver);
decode_results results;

void IRsetup() {
IrReceiver.begin(receiver, ENABLE_LED_FEEDBACK); // Start the receiver
}

void IRloop() {
if (IrReceiver.decode()) {
  uint32_t code = IrReceiver.decodedIRData.decodedRawData;
  Serial.print("IR received: 0x");
  Serial.println(code, HEX);
  
  if (code == 0xE916FF00) reset();
  
  IrReceiver.resume();
}

void reset(){
  count=0;
  buzz=0;
}
