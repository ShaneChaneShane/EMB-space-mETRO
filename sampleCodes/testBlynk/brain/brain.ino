#include <WiFi.h>
#include <esp_now.h>

// ===================== ESP-NOW PEER MAC (MINT) =====================
uint8_t MINT_MAC[] = {0x44, 0x1D, 0x64, 0xBE, 0x24, 0xCC};


#define RAIN_PIN   32   //umb
#define LED_MOTOR  33   //led


enum MsgType : uint8_t {
  MSG_RAIN=1, MSG_CMD=2,
  MSG_REQ_MOTOR=3, MSG_RES_MOTOR=4,
  MSG_REQ_STATE=5, MSG_RES_STATE=6,
  MSG_ACK=7
};

enum CmdVal : uint8_t { CMD_OPEN=1, CMD_CLOSE=2 };
enum MotorState : uint8_t { M_IDLE=0, M_OPENING=1, M_CLOSING=2, M_ERROR=3 };
enum UmbrellaState : uint8_t { U_UNKNOWN=0, U_OPEN=1, U_CLOSED=2 };

typedef struct __attribute__((packed)) {
  uint8_t type;
  uint8_t seq;
  uint8_t a;
  int16_t b;
} Packet;

volatile uint8_t seqTx = 0;

//states
MotorState motorState = M_IDLE;
UmbrellaState umbrellaState = U_CLOSED;

unsigned long lastRainSend = 0;

// simulation motor
bool actionRunning = false;
bool opening = false;
unsigned long actionStart = 0;

// helper
void sendPacket(uint8_t type, uint8_t a=0, int16_t b=0){
  Packet p;
  p.type = type;
  p.seq  = ++seqTx;
  p.a    = a;
  p.b    = b;
  esp_now_send(MINT_MAC, (uint8_t*)&p, sizeof(p));
}

//motor sim led
void startOpenLED(){
  motorState = M_OPENING;
  umbrellaState = U_UNKNOWN;

  opening = true;
  actionRunning = true;
  actionStart = millis();

  sendPacket(MSG_RES_MOTOR, motorState);
  Serial.println("[Brain] OPENING start");
}

void startCloseLED(){
  motorState = M_CLOSING;
  umbrellaState = U_UNKNOWN;

  opening = false;
  actionRunning = true;
  actionStart = millis();

  sendPacket(MSG_RES_MOTOR, motorState);
  Serial.println("[Brain] CLOSING start");
}

void finishAction(){
  actionRunning = false;
  motorState = M_IDLE;

  if(opening){
    umbrellaState = U_OPEN;
    digitalWrite(LED_MOTOR, HIGH);   // OPEN = LED ON
    Serial.println("[Brain] OPEN COMPLETE");
  } else {
    umbrellaState = U_CLOSED;
    digitalWrite(LED_MOTOR, LOW);    // CLOSED = LED OFF
    Serial.println("[Brain] CLOSE COMPLETE");
  }

  sendPacket(MSG_RES_MOTOR, motorState);
  sendPacket(MSG_RES_STATE, umbrellaState);
}

//esp now receive
void onRecv(const esp_now_recv_info_t* info, const uint8_t* data, int len){
  if(len != sizeof(Packet)) return;
  Packet p; 
  memcpy(&p, data, sizeof(p));

  switch(p.type){
    case MSG_CMD:
      sendPacket(MSG_ACK, p.seq);

      if(p.a == CMD_OPEN)  startOpenLED();
      if(p.a == CMD_CLOSE) startCloseLED();
      break;

    case MSG_REQ_MOTOR:
      sendPacket(MSG_RES_MOTOR, motorState);
      break;

    case MSG_REQ_STATE:
      sendPacket(MSG_RES_STATE, umbrellaState);
      break;
  }
}

void setup(){
  Serial.begin(115200);
  delay(300);

  WiFi.mode(WIFI_STA);

  if(esp_now_init() != ESP_OK){
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_peer_info_t peer{};
  memcpy(peer.peer_addr, MINT_MAC, 6);
  peer.channel = 0;
  peer.encrypt = false;

  if(esp_now_add_peer(&peer) != ESP_OK){
    Serial.println("Add peer failed");
    return;
  }

  esp_now_register_recv_cb(onRecv);

  pinMode(RAIN_PIN, INPUT_PULLUP);
  pinMode(LED_MOTOR, OUTPUT);
  digitalWrite(LED_MOTOR, LOW);

  Serial.println("[Brain Ready]");
}

void loop(){
  if(millis() - lastRainSend > 1000){
    lastRainSend = millis();

    int rainVal = digitalRead(RAIN_PIN);
    sendPacket(MSG_RAIN, 0, rainVal);

    Serial.print("[Rain] ");
    Serial.println(rainVal);
  }

//sim motor with led
  if(actionRunning){
    if(opening){
      digitalWrite(LED_MOTOR, (millis()/150)%2); 
    } else {
      digitalWrite(LED_MOTOR, (millis()/400)%2);
    }

    if(millis() - actionStart >= 3000){
      finishAction();
    }
  }
}
