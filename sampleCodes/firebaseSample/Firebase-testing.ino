#include<WiFi.h>
#include<Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "Moto"
#define WIFI_PASSWORD "87654321"
#define API_KEY "AIzaSyDmHL2GsIO3qv3ZUjLeKLAO8foffQ-7FiY"
#define DATABASE_URL "https://emb-space-metro-default-rtdb.asia-southeast1.firebasedatabase.app/"
// #define PWMChannel 0
#define LED1 26
#define LED2 27

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
FirebaseJson json;
bool signupOk = false;
void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
  Serial.println("ESP32 is running! and Connecting to WiFi");
  //connect wifi
  while(WiFi.status()!=WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }Serial.println("");
  Serial.print("Connected to WiFi IP: ");
  Serial.print(WiFi.localIP());


  // Sync Time
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

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  analogWrite(LED2, 20);
}

int counter = 1;
unsigned long sendDataPreMillis =0;

void TestWritting2(){
  if (Firebase.ready() && signupOk &&(millis() - sendDataPreMillis > 4000 || sendDataPreMillis == 0)) {
    sendDataPreMillis = millis();
    String path = "/TestWritting/day" + String(counter);

    json.set("value1", counter);
    json.set("value2", counter * 10);

    if (Firebase.RTDB.setJSON(&fbdo, path, &json)) {
      Serial.println("OK");
    } else {
      Serial.println(fbdo.errorReason());
    }

    counter++;
  }
}

bool readingBool=false;
int readingInt=0;
void TestReading(){
  if(Firebase.RTDB.getInt(&fbdo,"/TestReading/analog/")){
    if(fbdo.dataType()=="int"){
      readingInt=fbdo.intData();
      analogWrite(LED2, readingInt);
      Serial.println(readingInt);
    }
  }else{
    Serial.println(fbdo.errorReason());
  }
  if(Firebase.RTDB.getInt(&fbdo,"/TestReading/digital/")){
    if(fbdo.dataType()=="boolean"){
      readingBool=fbdo.boolData();
      if (readingBool) {
        digitalWrite(LED1, HIGH);
      } else {
        digitalWrite(LED1, LOW);
      }
      Serial.println(readingBool);
    }
  }else{
    Serial.println(fbdo.errorReason());
  }
}
void readFirst(int n) {
  FirebaseJson json;
  FirebaseJsonData data;

  for (int i = 0; i < n; i++) {
    String path = "/TestWritting/day" + String(i);

    if (Firebase.RTDB.getJSON(&fbdo, path)) {
      Serial.print("DAY ");
      Serial.println(i);
      Serial.println(fbdo.jsonString());
    }
    else {
      Serial.print("ERR: ");
      Serial.println(fbdo.errorReason());
    }
  }
}
void readLast(int lastIndex,int n) {
  int start = lastIndex - n+1;  // 

  for (int i = start; i <= lastIndex; i++) {
    String path = "/TestWritting/day" + String(i);

    if (Firebase.RTDB.getJSON(&fbdo, path)) {
      Serial.print("DAY ");
      Serial.println(i);
      Serial.println(fbdo.jsonString());
    }
    else {
      Serial.print("ERR: ");
      Serial.println(fbdo.errorReason());
    }
  }
}
void readBetween(int startDay, int endDay) {

  for (int i = startDay; i <= endDay; i++) {
    String path = "/TestWritting/day" + String(i);

    if (Firebase.RTDB.getJSON(&fbdo, path)) {
      Serial.print("DAY ");
      Serial.println(i);
      Serial.println(fbdo.jsonString());
    }
    else {
      Serial.print("ERR: ");
      Serial.println(fbdo.errorReason());
    }
  }
}



void loop() {
  TestWritting2();
  // TestReading();
  readBetween(10,20);

}



void TestWritting(){//don't use this
  if (Firebase.ready() && signupOk && millis() - sendDataPreMillis > 4000||sendDataPreMillis==0) {
    sendDataPreMillis = millis();


    if (Firebase.RTDB.pushInt(&fbdo, "/TestWritting/value", counter)) {// in Realtime Database /TestWritting has value:counter
      Serial.println("OK");
    } else {
      Serial.println(fbdo.errorReason());
    }
    counter++;      
  }
}


                                                                                                                                                                      