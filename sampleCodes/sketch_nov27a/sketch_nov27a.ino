#include <WiFi.h>
#include <esp_now.h>

void OnDataRecv(const esp_now_recv_info * info, const uint8_t *incomingData, int len) {
  char macStr[18];
  const uint8_t * mac = info->src_addr;

  snprintf(macStr, sizeof(macStr),
           "%02X:%02X:%02X:%02X:%02X:%02X",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  Serial.print("From: ");
  Serial.println(macStr);

  Serial.print("Data: ");
  Serial.write(incomingData, len);
  Serial.println();
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed!");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);

  Serial.print("Brain MAC Address: ");
  Serial.println(WiFi.macAddress());
}

void loop() {

}
