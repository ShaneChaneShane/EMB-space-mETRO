#include <DHT.h>     // ← ต้องเป็นแบบนี้
#define PIN 23
#define TYPE DHT11

DHT dht(PIN, TYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
}

void loop() {
  float temp = dht.readTemperature();

  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println(" °C");

  delay(1000);
}
