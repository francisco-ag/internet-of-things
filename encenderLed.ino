int ledPin = 12;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hola mundo, ESP32!");
}

void loop() {
  digitalWrite(ledPin, HIGH);
  delay(10000);
}