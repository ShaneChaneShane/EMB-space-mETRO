#define BLYNK_TEMPLATE_ID "TMPL6T0nHp_OC"
#define BLYNK_TEMPLATE_NAME "test"
#define BLYNK_AUTH_TOKEN "ySDWkab4D1FH10wZajhC_w89sxmfY8jT"

#include "globals.hpp"
#include "payloads.hpp"

#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <BlynkSimpleEsp32.h>

// sensors
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"
#include <BlynkSimpleEsp32.h>   // you already have this, keep only one

// --- Sensor to pin mapping (from mint_sensors_adding_firebase.ino) ---
// V0: temp
// V1: humid1
// V2: humid2
// V3: light raw
// V4: coverState (0=UNKNOWN,1=RETRACTED,2=EXTENDED)
// V5: motorState (0=HALT,1=WORKING)
// V6: rainingState (0=DRY,1=RAINING)
// V7: close/open to brain

#define DHTTYPE      DHT11
#define DHTPIN1      27
#define DHTPIN2      26
#define ONE_WIRE_BUS 25
#define LIGHT_AO_PIN 34

DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds18b20(&oneWire);

// Blynk timer for periodic sending
BlynkTimer timer;


CoverState   coverState   = UNKNOWN;
MotorState   motorState   = HALT;
RainingState rainingState = DRY;

TaskHandle_t espNowTaskHandler;
QueueHandle_t espNowQueue;

//mac
uint8_t MINT_MAC[]  = { 0x44, 0x1D, 0x64, 0xBE, 0x24, 0xCC };  // THIS BOARD
uint8_t BRAIN_MAC[] = { 0x08, 0xA6, 0xF7, 0xB1, 0xD8, 0xB4 };
uint8_t BEAU_MAC[]  = { 0xD0, 0xCF, 0x13, 0x15, 0x4A, 0x2C };

Packet receivedPacket;
esp_now_peer_info_t peerInfo;
static uint16_t seqTx = 0;

// line -----------------------
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
// ----------------------------


CoverState lastCoverState = UNKNOWN;
// ESP NOW ----------------------------
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? 
                 "Delivery Success" : "Delivery Fail");
}

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


  WiFi.begin("Hi", "Noon12345");
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
  Serial.println("Successfully add peer");
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
  
  Serial.println("Successfully add Queue Task");
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
  
  Serial.println("Successfully send cmd to BRAIN");
}
// ----------------------------
// BLYNK
BLYNK_WRITE(V7) {
  int v = param.asInt();

  if (v == 1) sendCmdToBrain(true);   // EXTEND
  else        sendCmdToBrain(false);  // RETRACT
}

BLYNK_CONNECTED() {
  Blynk.syncAll();
}
void sendSensorsToBlynk() {
  // Read sensors
  float h1 = dht1.readHumidity();
  float h2 = dht2.readHumidity();
  Serial.println("READ HUMIDITY 1 AND 2");
  Serial.println(h1);
  Serial.println(h2);
  Serial.println("----------");
  ds18b20.requestTemperatures();
  float t_ds = ds18b20.getTempCByIndex(0);
  Serial.println("READ TEMPERATURE");
  Serial.println(t_ds);
  Serial.println("----------");
  int lightRaw = analogRead(LIGHT_AO_PIN);
  Serial.println("READ LIGHT");
  Serial.println(lightRaw);
  Serial.println("----------");
  // Send to Blynk (same mapping as mint_sensors_adding_firebase.ino)
  if (!isnan(h1)) {
    Blynk.virtualWrite(V1, h1);
    Serial.println("BLYNKL: WROTE HUMIDITY 1 (CLOTHES) TO V1");
  }
  if (!isnan(h2)) {
    Blynk.virtualWrite(V2, h2);
    Serial.println("BLYNKL: WROTE HUMIDITY 2 (ENV) TO V2");
  }
  if (!isnan(t_ds)) {
    Blynk.virtualWrite(V0, t_ds);
    Serial.println("BLYNKL: WROTE TEMP TO V0");
  }


  Blynk.virtualWrite(V3, lightRaw);
  Serial.println("BLYNKL: WROTE LIGHT TO V3");

  // States coming from the “brain” / ESP-NOW logic
  Blynk.virtualWrite(V4, (int)coverState);
  Blynk.virtualWrite(V5, (int)motorState);
  Blynk.virtualWrite(V6, (int)rainingState);
}

void setup() {
  Serial.begin(115200);

  // Initialize ESP-NOW and WiFi
  setupEspNow();

  // Blynk: use existing WiFi connection
  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect();

  // --- Sensor init
  dht1.begin();
  dht2.begin();
  ds18b20.begin();
  pinMode(LIGHT_AO_PIN, INPUT);

  // Send sensors & states to Blynk every 1 second
  timer.setInterval(1000L, sendSensorsToBlynk);
}

void loop() {
  // Let Blynk handle communication, widgets, etc.
  Blynk.run();

  // Run BlynkTimer (calls sendSensorsToBlynk every 1s)
  timer.run();

  // Small delay so loop isn’t too tight
  delay(10);
}
