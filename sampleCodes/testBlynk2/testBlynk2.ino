//testBlynk2.ino
//blynkk
#define BLYNK_TEMPLATE_ID   "TMPL6T0nHp_OC"
#define BLYNK_TEMPLATE_NAME "test"
#define BLYNK_AUTH_TOKEN    "ySDWkab4D1FH10wZajhC_w89sxmfY8jT"
#define BLYNK_PRINT Serial

#include "globals.hpp"
#include "payloads.hpp"

#include <WiFi.h>
#include <esp_now.h>
#include <BlynkSimpleEsp32.h>


char ssid[] = "AAA";        
char pass[] = "11111111";   
 
//define
CoverState   coverState   = UNKNOWN;
MotorState   motorState   = HALT;
RainingState rainingState = DRY;

//q
QueueHandle_t eventQueue  = nullptr;

TaskHandle_t sensorTaskHandler;

BlynkTimer timer;

//pht
void setupWifiAndBlynk();
void setupSensorsMint();
void readSensorsTask(void *pvParameters);
void setupEspNow();  
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);

void setup() {
  Serial.begin(115200);
  delay(100);

  setupWifiAndBlynk();   //setupblynk
  setupSensorsMint();  
  setupEspNow();         

  //read up to blynk 1 sec
  xTaskCreatePinnedToCore(
    readSensorsTask,
    "sensor-task",
    8192,
    NULL,
    1,
    &sensorTaskHandler,
    1
  );
}

void loop() {
  Blynk.run();
  timer.run();
}

// wifi+blynk
void setupWifiAndBlynk() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("WiFi connected, channel = ");
  Serial.println(WiFi.channel());

  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect();
}
