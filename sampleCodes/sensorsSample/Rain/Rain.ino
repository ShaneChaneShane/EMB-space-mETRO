const int RAIN_PIN = 23;   // เลือกได้ GPIO32/33/34/35

void setup() {
  Serial.begin(115200);
  pinMode(RAIN_PIN, INPUT_PULLDOWN);
}

void loop() {
  int rain = digitalRead(RAIN_PIN);

  if (rain == HIGH) {
    Serial.println("No Rain");
  } else {
    Serial.println("Rain Detected");
  }

  delay(200);
}
