// mint_sensors.ino

//mapping
// V0: humid1
// V1: humid2
// V2: temp
// V3: light raw
// V4: coverState (0=UNKNOWN,1=RETRACTED,2=EXTENDED)
// V5: motorState (0=HALT,1=WORKING)
// V6: rainingState (0=DRY,1=RAINING)
// V7: close/open to brain
#include "globals.hpp"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"
#include <BlynkSimpleEsp32.h>

#include<WiFi.h>
#include<Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#define API_KEY "AIzaSyDmHL2GsIO3qv3ZUjLeKLAO8foffQ-7FiY"
#define DATABASE_URL "https://emb-space-metro-default-rtdb.asia-southeast1.firebasedatabase.app/"
#include <RTClib.h>
RTC_DS3231 rtc;
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
FirebaseJson json;
bool signupOk = false;
//pins
#define DHTTYPE      DHT11
#define DHTPIN1      27
#define DHTPIN2      26
#define ONE_WIRE_BUS 25
#define LIGHT_AO_PIN 34

DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds18b20(&oneWire);

//states globals.hpp
extern CoverState   coverState;
extern MotorState   motorState;
extern RainingState rainingState;
String makeKey(DateTime now);

void setupSensorsMint() {
  dht1.begin();
  dht2.begin();
  ds18b20.begin();

  pinMode(LIGHT_AO_PIN, INPUT);




  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  Serial.print("Syncing time");
  while(time(nullptr) < 1700000000){
      Serial.print(".");
      delay(200);
  }
  Serial.println("\nTime synced!");
  //connect firebase
  config.api_key=API_KEY;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")) {

      Serial.println("Signup Firebase okay");
      signupOk=true;
  }else{
    Serial.printf("%s\n",config.signer.signupError.message.c_str());
  }
  config.token_status_callback=tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  rtc.begin();
}
String makeKey(DateTime now) {
  return String(now.unixtime()) + "_" + String(millis());
}


//read every 1 sec
void readSensorsTask(void *pvParameters) {
  for (;;) {
    DateTime now = rtc.now();

    String datetime =  makeKey(now);

    float h1 = dht1.readHumidity();
    float h2 = dht2.readHumidity();

    ds18b20.requestTemperatures();
    float t_ds = ds18b20.getTempCByIndex(0);

    int lightRaw = analogRead(LIGHT_AO_PIN);

    // not Nan
    if (!isnan(h1)) {
      Blynk.virtualWrite(V0, h1);
      json.set("Temp", h1);
    }
    if (!isnan(h2)) {
      Blynk.virtualWrite(V1, h2);
      json.set("Humidity_Clothes", h2);
    }
    if (!isnan(t_ds)) {
      Blynk.virtualWrite(V2, t_ds);
      json.set("Humidity_Env", t_ds);
    }

    Blynk.virtualWrite(V3, lightRaw);
    json.set("Light", lightRaw);

    // stage brain to blynk
    Blynk.virtualWrite(V4, (int)coverState);
    json.set("Rain", (int)coverState);

    Blynk.virtualWrite(V5, (int)motorState);
    json.set("MotorInProcess", (int)motorState);

    Blynk.virtualWrite(V6, (int)rainingState);
    json.set("RainProtectorStatus", (int)rainingState);

    String path = "/RealtimeData/" + datetime;
    if (Firebase.RTDB.setJSON(&fbdo, path, &json)) {
      Serial.println("OK");
    } else {
      Serial.println(fbdo.errorReason());
    }

    
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
