void setup() {
  motorSetup();
  encoderSetup();
}

void loop() {
  encoderLoop();
  // Example Usage:
  startForward();
  // delay(2048); // Spin forward for roughly one revolution
  
  // stopMotor();
  // delay(1000); // Wait a second
  
  // startReverse();
  // delay(2048); // Spin back
  
  // stopMotor();
  // delay(5000); // Wait 5 seconds
}
