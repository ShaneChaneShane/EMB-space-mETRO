#ifndef FIREBASE_HPP
#define FIREBASE_HPP

#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// WiFi
#ifndef WIFI_SSID
  #define WIFI_SSID     
#endif

#ifndef WIFI_PASSWORD
  #define WIFI_PASSWORD "wifipass"
#endif

// Firebase

#ifndef FIREBASE_API_KEY
  #define FIREBASE_API_KEY "firebase"
#endif

#ifndef FIREBASE_DB_URL
  #define FIREBASE_DB_URL "url"
#endif

#ifndef FIREBASE_USER_EMAIL
  #define FIREBASE_USER_EMAIL "email"
#endif

#ifndef FIREBASE_USER_PASSWORD
  #define FIREBASE_USER_PASSWORD "password"
#endif


static FirebaseData fbdo;
static FirebaseAuth fbAuth;
static FirebaseConfig fbConfig;

static bool firebaseReady = false;
static unsigned long firebaseLastSendMs = 0;
static unsigned long firebaseSendIntervalMs = 5000; 


inline void connectWiFi()
{
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.print("Connecting to WiFi: ");
  Serial.print(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  uint8_t retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 40) {
    Serial.print(".");
    delay(500);
    retries++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi connected. IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi connection FAILED!");
  }
}

// setup

inline void firebaseSetup()
{
  Serial.println();
  Serial.println("=== Firebase Setup ===");

  connectWiFi();

  // config
  fbConfig.api_key = FIREBASE_API_KEY;
  fbConfig.database_url = FIREBASE_DB_URL;

  fbAuth.user.email = FIREBASE_USER_EMAIL;
  fbAuth.user.password = FIREBASE_USER_PASSWORD;



  Firebase.begin(&fbConfig, &fbAuth);
  Firebase.reconnectWiFi(true);

  unsigned long start = millis();
  while (!Firebase.ready() && millis() - start < 10000) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();

  if (Firebase.ready()) {
    firebaseReady = true;
    Serial.println("Firebase ready!");
  } else {
    firebaseReady = false;
    Serial.println("Firebase NOT ready!");
  }
}


inline bool firebaseSetFloat(const String &path, float value)
{
  if (!firebaseReady || WiFi.status() != WL_CONNECTED) {
    Serial.println("[Firebase] Not ready or WiFi disconnected");
    return false;
  }

  if (Firebase.RTDB.setFloat(&fbdo, path.c_str(), value)) {
    Serial.print("[Firebase] SET float OK: ");
    Serial.print(path);
    Serial.print(" = ");
    Serial.println(value);
    return true;
  } else {
    Serial.print("[Firebase] SET float FAILED: ");
    Serial.println(fbdo.errorReason());
    return false;
  }
}


inline bool firebaseGetFloat(const String &path, float &outValue)
{
  if (!firebaseReady || WiFi.status() != WL_CONNECTED) {
    Serial.println("[Firebase] Not ready or WiFi disconnected");
    return false;
  }

  if (Firebase.RTDB.getFloat(&fbdo, path.c_str())) {
    outValue = fbdo.floatData();
    Serial.print("[Firebase] GET float OK: ");
    Serial.print(path);
    Serial.print(" = ");
    Serial.println(outValue);
    return true;
  } else {
    Serial.print("[Firebase] GET float FAILED: ");
    Serial.println(fbdo.errorReason());
    return false;
  }
}

//loop

inline void firebaseHandle()
{
  // เช็ค WiFi ตลอด
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!firebaseReady || !Firebase.ready()) {
    firebaseSetup();
    return;
  }


  if (millis() - firebaseLastSendMs >= firebaseSendIntervalMs) {
    firebaseLastSendMs = millis();

    //exam sensor
    float temperature = random(250, 350) / 10.0; // 25.0 - 35.0
    float humidity    = random(400, 800) / 10.0; // 40.0 - 80.0
    firebaseSetFloat("/sensor1/temperature", temperature);
    firebaseSetFloat("/sensor1/humidity", humidity);
  }
}

#endif // FIREBASE_HPP
