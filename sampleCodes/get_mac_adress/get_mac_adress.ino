#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  delay(100);

  Serial.print("Beau MAC Address: ");
  Serial.println(WiFi.macAddress());
}

void loop() {
  
}
