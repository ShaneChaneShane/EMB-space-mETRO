#include "globals.hpp"
#include "payloads.hpp"
#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>

TaskHandle_t espNowTaskHandler;
QueueHandle_t espNowQueue;

esp_now_peer_info_t peerInfo1 = {};
esp_now_peer_info_t peerInfo2 = {};
Packet receivedPacket;

uint8_t MINT_MAC[] = { 0x44, 0x1D, 0x64, 0xBE, 0x24, 0xCC };
uint8_t BRAIN_MAC[] = { 0x28, 0x56, 0x2F, 0x49, 0x6A, 0x54 };
uint8_t BEAU_MAC[] = { 0xD0, 0xCF, 0x13, 0x15, 0x4A, 0x2C };

void setupEspNow() {
  WiFi.mode(WIFI_STA);
  WiFi.begin("AAA", "11111111");
  Serial.print("Waiting for WiFi init");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(10);
  } 
  Serial.print("WiFi channel: ");
  Serial.println(WiFi.channel());

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW initialization failed");
    return;
  }

  esp_now_register_send_cb(esp_now_send_cb_t(OnDataSent));

  memcpy(peerInfo1.peer_addr, MINT_MAC, 6);
  peerInfo1.channel = 0;
  peerInfo1.encrypt = false;
  peerInfo2.channel = 0;
  peerInfo2.encrypt = false;

  if (esp_now_add_peer(&peerInfo1) != ESP_OK) {
    Serial.println("Failed to add peer 1 (MINT)");
    return;
  } else {
    Serial.println("Peer added! (MINT)");
  }

  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  espNowQueue = xQueueCreate(16, sizeof(Packet));
  xTaskCreatePinnedToCore(espNowTask, "esp-now-task", 8192, NULL, 2, &espNowTaskHandler, 0);
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  Serial.print("Bytes received: ");
  Serial.println(len);

  if (len < (int)sizeof(Packet)) {
    Serial.println("Error: Data size too small for Packet");
    return;
  }

  memcpy(&receivedPacket, incomingData, sizeof(Packet));

  // if (memcmp(mac_addr, MINT_MAC, 6) == 0 || memcmp(mac_addr, BEAU_MAC, 6) == 0) {
    EventType event;
    switch (receivedPacket.header.type) {
      case MSG_CMD:
         event = (receivedPacket.payload.cmd.openCover) ? CMD_EXTEND : CMD_RETRACT;
        xQueueSend(eventQueue, &event, portMAX_DELAY);
        break;
      case MSG_STATE:
        Serial.println("Unexpected message type received");
        break;
      case MSG_SENSOR:
        Serial.println("Unexpected message type received");
        break;
      case MSG_ACK:
        Serial.println("Unexpected message type received");
        break;
      default:
        Serial.println("Unexpected/Undefined message type received");
        break;
    }
  // } else {
  //   Serial.println("Unknown source of received data");
  // }
}


void espNowTask(void *pvParameters) {
  Packet p;
  for (;;) {
    if (xQueueReceive(espNowQueue, &p, portMAX_DELAY)) {
      switch (p.header.dst) {
        case MINT:
          if (esp_now_send(MINT_MAC, (uint8_t *)&p, sizeof(p)) != ESP_OK) {
            Serial.println("Error sending data");
          }
          break;
        // case BEAU: result = esp_now_send(BEAU_MAC, (uint8_t *) &p, sizeof(p));
        // case BRAIN: result = esp_now_send(BRAIN_MAC, (uint8_t *) &p, sizeof(p));
        default: Serial.println("Illegal Operation for this board.");
      }
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}