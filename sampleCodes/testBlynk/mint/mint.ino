#define BLYNK_TEMPLATE_ID   "TMPL6T0nHp_OC"
#define BLYNK_TEMPLATE_NAME "test"
#define BLYNK_AUTH_TOKEN    "ySDWkab4D1FH10wZajhC_w89sxmfY8jT"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"
#include <esp_now.h>


#define DHTTYPE  DHT11
#define DHTPIN1  27
#define DHTPIN2  26
#define LIGHT_AO_PIN 34
#define ONE_WIRE_BUS 25

DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds18b20(&oneWire);

BlynkTimer timer;

char ssid[] = "AAA";
char pass[] = "11111111";

// ===================== ESP-NOW PEER MAC (BRAIN) =====================
uint8_t BRAIN_MAC[] = {0x08, 0xA6, 0xF7, 0xB1, 0xD8, 0xB4};

enum MsgType : uint8_t {
  MSG_RAIN=1, MSG_CMD=2,
  MSG_REQ_MOTOR=3, MSG_RES_MOTOR=4,
  MSG_REQ_STATE=5, MSG_RES_STATE=6,
  MSG_ACK=7
};
enum CmdVal : uint8_t { CMD_OPEN=1, CMD_CLOSE=2 };
enum UmbrellaState : uint8_t { U_UNKNOWN=0, U_OPEN=1, U_CLOSED=2 };

typedef struct __attribute__((packed)) {
  uint8_t type;
  uint8_t seq;
  uint8_t a;
  int16_t b;
} Packet;

volatile uint8_t seqTx = 0;


//auto or manual states
bool manualOverride = false;
String umbrellaText = "CLOSED";

//buffer brin
volatile int latestRainVal = 1;
volatile bool rainUpdated = false;

volatile uint8_t latestUmbState = U_CLOSED;
volatile bool umbUpdated = false;


volatile bool ignoreNextV7 = false;

//helper
String umbToText(uint8_t u){
  if(u==U_OPEN) return "OPEN";
  if(u==U_CLOSED) return "CLOSED";
  return "UNKNOWN";
}

void sendPacket(uint8_t type, uint8_t a=0, int16_t b=0){
  Packet p;
  p.type = type;
  p.seq  = ++seqTx;
  p.a    = a;
  p.b    = b;
  esp_now_send(BRAIN_MAC, (uint8_t*)&p, sizeof(p));
}

//espnow receive
void onEspNowRecv(const esp_now_recv_info_t* info, const uint8_t* data, int len){
  if(len != sizeof(Packet)) return;
  Packet p; memcpy(&p, data, sizeof(p));

  switch(p.type){
    case MSG_RAIN:
      latestRainVal = p.b;
      rainUpdated = true;
      Serial.print("[ESP-NOW] Rain: ");
      Serial.println(latestRainVal);
      break;

    case MSG_RES_STATE:
      latestUmbState = p.a;
      umbUpdated = true;
      Serial.print("[ESP-NOW] Umbrella state raw: ");
      Serial.println(latestUmbState);
      break;

    case MSG_ACK:
      Serial.print("[ESP-NOW] ACK seq=");
      Serial.println(p.a);
      break;
  }
}

//PUSH ESP-NOW DATA TO BLYNK
void pushEspNowToBlynk(){
  //rain
  if(rainUpdated){
    rainUpdated = false;
    int rainVal = latestRainVal;

    Blynk.virtualWrite(V4, rainVal);

    if (rainVal == 0 && !manualOverride) {
      Serial.println("[AUTO] Rain detected -> OPEN");
      sendPacket(MSG_CMD, CMD_OPEN);
    }

    if (rainVal == 1 && manualOverride) {
      Serial.println("[AUTO] Rain stopped -> manualOverride OFF");
      manualOverride = false;
    }
  }

  //Umbrella -> V7 visual
  if(umbUpdated){
    umbUpdated = false;
    umbrellaText = umbToText(latestUmbState);

    Serial.print("[SYNC] UmbrellaText = ");
    Serial.println(umbrellaText);

    ignoreNextV7 = true;   
    if (umbrellaText == "OPEN") {
      Blynk.virtualWrite(V7, 1);
    } else if (umbrellaText == "CLOSED") {
      Blynk.virtualWrite(V7, 0);
    }
  }
}

//sync ร่ม
void requestState(){
  sendPacket(MSG_REQ_STATE);
}

//BLYNK TOGGLE BUTTON (V7)
BLYNK_WRITE(V7){
  if(ignoreNextV7){     
    ignoreNextV7 = false;
    return;
  }

  if(param.asInt()){
    manualOverride = true;

    if (umbrellaText == "OPEN") {
      Serial.println("[MANUAL] CLOSE");
      sendPacket(MSG_CMD, CMD_CLOSE);
    } 
    else if (umbrellaText == "CLOSED") {
      Serial.println("[MANUAL] OPEN");
      sendPacket(MSG_CMD, CMD_OPEN);
    } 
    else {
      Serial.println("[MANUAL] UNKNOWN -> request state");
      sendPacket(MSG_REQ_STATE);
    }
  }
}

//SENSOR
void sendSensor() {
  float h1 = dht1.readHumidity();
  float h2 = dht2.readHumidity();

  ds18b20.requestTemperatures();
  float t_ds = ds18b20.getTempCByIndex(0);

  int lightRaw = analogRead(LIGHT_AO_PIN);

  Blynk.virtualWrite(V0, t_ds);
  Blynk.virtualWrite(V1, h1);
  Blynk.virtualWrite(V2, h2);
  Blynk.virtualWrite(V3, lightRaw);
}

// ===================== SETUP / LOOP =====================
void setup() {
  Serial.begin(115200);
  delay(500);

  dht1.begin();
  dht2.begin();
  ds18b20.begin();
  analogSetPinAttenuation(LIGHT_AO_PIN, ADC_11db);

  WiFi.mode(WIFI_STA);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed!");
    return;
  }

  esp_now_peer_info_t peer{};
  memcpy(peer.peer_addr, BRAIN_MAC, 6);
  peer.channel = 0;
  peer.encrypt = false;
  esp_now_add_peer(&peer);

  esp_now_register_recv_cb(onEspNowRecv);

  timer.setInterval(2000L, sendSensor);
  timer.setInterval(3000L, requestState);
  timer.setInterval(500L, pushEspNowToBlynk);
}

void loop() {
  Blynk.run();
  timer.run();
}
