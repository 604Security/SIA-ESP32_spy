// petbot Phase 0: blink the XIAO ESP32S3 user LED.
// The orange user LED is on GPIO21 (LED_BUILTIN) and is active LOW.
// GPIO21 is also the microSD CS pin, so don't use the LED together with the SD card.

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, LOW);   // on
  Serial.println("LED on");
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);  // off
  Serial.println("LED off");
  delay(500);
}
