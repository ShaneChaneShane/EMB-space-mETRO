#include<WiFi.h>
#include<Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "Moto"
#define WIFI_PASSWORD "87654321"
#define API_KEY "AIzaSyDmHL2GsIO3qv3ZUjLeKLAO8foffQ-7FiY"
#define DATABASE_URL "https://emb-space-metro-default-rtdb.asia-southeast1.firebasedatabase.app/"

// #define LDR_PIN  <---

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
unsigned long sendDataPreMillis =0;
bool signupOk = false;
int ldrData =0;
float voltage=0.0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID,WIFI_PASSWORD);

  Serial.println("ESP32 is running! and Connecting to WiFi");

  while(WiFi.status()!=WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("Connected to WiFi IP: ");
  Serial.print(WiFi.localIP());

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

  analogWrite(2, 50);  

}

int counter = 1;
void loop() {
  if (Firebase.ready() && signupOk && millis() - sendDataPreMillis > 4000||sendDataPreMillis==0) {
    sendDataPreMillis = millis();


    if (Firebase.RTDB.setInt(&fbdo, "/test/value", counter)) {
      Serial.println("OK");
    } else {
      Serial.println(fbdo.errorReason());
    }
    counter++;      
  }
}





// int ledPin = 2;   

// void setup() {
//   Serial.begin(115200);
//   delay(1000); 
//   Serial.println("ESP32 is running!");

//   pinMode(ledPin, OUTPUT);
// }

// void loop() {
//   digitalWrite(ledPin, HIGH);  
//   Serial.println("LED ON");
//   delay(1000);

//   digitalWrite(ledPin, LOW);    
//   Serial.println("LED OFF");
//   delay(1000);
// }
                                                                                                                                                                     