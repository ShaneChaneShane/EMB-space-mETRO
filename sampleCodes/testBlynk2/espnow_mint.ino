// espnow_mint.ino

#include "globals.hpp"
#include "payloads.hpp"

#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <HTTPClient.h>

TaskHandle_t espNowTaskHandler;
QueueHandle_t espNowQueue;

//mac
uint8_t MINT_MAC[]  = { 0x44, 0x1D, 0x64, 0xBE, 0x24, 0xCC };  // THIS BOARD
uint8_t BRAIN_MAC[] = { 0x08, 0xA6, 0xF7, 0xB1, 0xD8, 0xB4 };
uint8_t BEAU_MAC[]  = { 0xD0, 0xCF, 0x13, 0x15, 0x4A, 0x2C };

Packet receivedPacket;
esp_now_peer_info_t peerInfo;
static uint16_t seqTx = 0;

// line
String LINE_TOKEN = "+Awb8i1H9s7XzsTg89412DCVvYwAXnwOryF4h0zKOSyBuvlf8/8a87jGS0n7C+BTDAtOzcmaYMY5gkqVNFbbs9Dr+ilsdxtfB+WolEtRqtNbEti4sAvGJHHsWAm8PUm35fCkC4GOgLBZYlEHEAQS5QdB04t89/1O/w1cDnyilFU="; // Channel access token
String GROUP_ID = "Ca28231dfd82d32667d5c5c81756ccbfb";

void sendLineMessage(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("https://api.line.me/v2/bot/message/push");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + LINE_TOKEN);

    String payload = "{\"to\":\"" + GROUP_ID + "\",\"messages\":[{\"type\":\"text\",\"text\":\"" + message + "\"}]}";
    int httpResponseCode = http.POST(payload);

    Serial.print("LINE Response: ");
    Serial.println(httpResponseCode);
    http.end();
  }
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? 
                 "Delivery Success" : "Delivery Fail");
}

CoverState lastCoverState = UNKNOWN;
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {

  if (len > sizeof(receivedPacket)) {
    Serial.println("Error: Data size exceeds expected packet size!");
    return;
  }

  memcpy(&receivedPacket, incomingData, sizeof(receivedPacket));

  Serial.print("Bytes received: ");
  Serial.println(len);

 
  if (memcmp(mac_addr, BRAIN_MAC, 6) == 0) {

    if (receivedPacket.header.type == MSG_STATE) {

      coverState   = receivedPacket.payload.state.coverState;
      motorState   = receivedPacket.payload.state.motorState;
      rainingState = receivedPacket.payload.state.rainingState;

      Serial.println("STATE UPDATE FROM BRAIN:");
      Serial.printf("Cover=%d Motor=%d Rain=%d\n",
                    coverState, motorState, rainingState);

   
      Blynk.virtualWrite(V4, (int)coverState);
      Blynk.virtualWrite(V5, (int)motorState);
      Blynk.virtualWrite(V6, (int)rainingState);

      if (coverState != lastCoverState) {
        lastCoverState = coverState;
        String stringToSend = "Cover changed to: ";
        switch (coverState) {
          case RETRACTED:
            stringToSend = stringToSend + "RETRACTED";
            break;
          case EXTENDED:
            stringToSend = stringToSend + "EXTENDED";
            break;
          default:
            stringToSend = stringToSend + "UNKNOWN";
            break;
        }
        sendLineMessage(stringToSend);
      }

    } else {
      Serial.println("Unexpected Message Type from BRAIN");
    }

  } else {
    Serial.println("Unknown source of received data");
  }
}

void espNowTask(void *pvParameters) {

  Packet p;

  for (;;) {
    if (xQueueReceive(espNowQueue, &p, portMAX_DELAY)) {

      switch (p.header.dst) {

      case BRAIN:
        if (esp_now_send(BRAIN_MAC, (uint8_t*)&p, sizeof(p)) != ESP_OK) {
          Serial.println("Error sending to BRAIN");
        }
        break;

      default:
        Serial.println("Illegal Destination for MINT");
      }
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void setupEspNow() {

  WiFi.mode(WIFI_STA);


  WiFi.begin("AAA", "11111111");
  while (WiFi.status() != WL_CONNECTED) delay(10);

  Serial.print("WiFi channel: ");
  Serial.println(WiFi.channel());

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW initialization failed");
    return;
  }

  // Register callbacks
  esp_now_register_send_cb(esp_now_send_cb_t(OnDataSent));
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  // Set peer = BRAIN
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, BRAIN_MAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // Queue + Task
  espNowQueue = xQueueCreate(16, sizeof(Packet));
  xTaskCreatePinnedToCore(
      espNowTask,
      "esp-now-task",
      8192,
      NULL,
      2,
      &espNowTaskHandler,
      1  // ย้ายไป core1 เพื่อไม่ชน sensor task
  );
}


void sendCmdToBrain(bool openCover) {

  Packet p;

  p.header.src  = MINT;
  p.header.dst  = BRAIN;
  p.header.type = MSG_CMD;
  p.header.seq  = ++seqTx;

  p.payload.cmd.openCover = openCover;

  if (esp_now_send(BRAIN_MAC, (uint8_t*)&p, sizeof(p)) != ESP_OK) {
    Serial.println("Error sending CMD to BRAIN");
  }
}

BLYNK_WRITE(V7) {
  int v = param.asInt();

  if (v == 1) sendCmdToBrain(true);   // EXTEND
  else        sendCmdToBrain(false);  // RETRACT
}

BLYNK_CONNECTED() {
  Blynk.syncAll();
}
