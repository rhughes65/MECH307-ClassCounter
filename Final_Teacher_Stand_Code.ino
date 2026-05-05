#include <SPI.h>
#include <nRF24L01.h>  //Part of the RF24 library
#include <RF24.h>
#include <LiquidCrystal.h>// Include lcd library
const int rs = 10, en = 8, d4 = 7, d5 = 6, d6 = 5, d7 = 4;// Set up pins
LiquidCrystal lcd(rs,en,d4,d5,d6,d7);// Set up lcd function 
// Define NRF24L01 radio object with CE and CSN pins
RF24 radio(12, 13);  // CE = 12, CSN = 13

// Define the pipe address (must match the sender)
const byte address[6] = "00001";

struct RF_Data {
  int personCount; 
  int buzz;
  //Put other variables in here that you want to pass via RF
};
int buzzer=11;
int personCount=1;
int buzz=0;
int test=12000;
int prevcount=0;
void setup() {
  lcd.begin(16,2);
  Serial.begin(9600);
  pinMode(buzzer,OUTPUT);
  radio.begin();
  radio.setChannel(30);                // Use the same channel as sender
  radio.setPALevel(RF24_PA_HIGH);     // Power level (HIGH for better range), switch to low if experiencing soft resets
  radio.setDataRate(RF24_250KBPS);    // Lower data rate = longer range
  radio.openReadingPipe(0, address);  // Set receiver address
  radio.startListening();      
  lcd.setCursor(0, 0);
  lcd.print("MECH 307");
  lcd.setCursor(0, 1); 
  lcd.print("Students: ");       // Set as receiver
}

void loop() {
 
if (radio.available()) {
    RF_Data data;
    radio.read(&data, sizeof(data));  
    personCount=data.personCount;
    buzz=data.buzz;
  if (personCount != prevcount){
  lcd.setCursor(10,1);
  lcd.print("  ");
  lcd.setCursor(10,1);
  lcd.print(personCount);
   if (buzz==5){
    tone(buzzer, 500, 500);
  }
  }
  Serial.println(test);
 
  prevcount=personCount;
  }

}
