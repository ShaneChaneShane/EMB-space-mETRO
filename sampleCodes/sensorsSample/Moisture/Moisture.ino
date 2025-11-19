const int LIGHT_PIN = 23;

void setup() {
  Serial.begin(115200);
  pinMode(LIGHT_PIN, INPUT);
}

void loop() {
  int value = digitalRead(LIGHT_PIN); 
  Serial.println(value);               
  delay(200);
}
