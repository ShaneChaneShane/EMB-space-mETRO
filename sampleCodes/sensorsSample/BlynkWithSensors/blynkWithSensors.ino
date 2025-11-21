#define BLYNK_TEMPLATE_ID   "TEMPLATE ID HERE"
#define BLYNK_TEMPLATE_NAME "TEMPLATE NAME HERE"
#define BLYNK_AUTH_TOKEN    "BLYNK AUTH TOKEN HERE"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"


#define BLYNK_PRINT Serial
#define DHTTYPE  DHT11
#define DHTPIN1  27
#define DHTPIN2  26
#define LIGHT_AO_PIN 34
#define ONE_WIRE_BUS 33


BlynkTimer timer;
DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds18b20(&oneWire);
char ssid[] = "WIFI NAME HERE";
char pass[] = "WIFI PASS HERE";


void sendSensor() {
  // ---- DHT11 ----
  float h1 = dht1.readHumidity();
  float h2 = dht2.readHumidity();

  if (isnan(h1)) Serial.println("DHT11-1 read error");
  if (isnan(h2)) Serial.println("DHT11-2 read error");

  // ---- DS18B20 ----
  ds18b20.requestTemperatures();
  float t_ds = ds18b20.getTempCByIndex(0);

  // ---- Light (Analog RAW 0-4095) ----
  int lightRaw = analogRead(LIGHT_AO_PIN);

  // ---- Debug Serial ----
  Serial.println("-------------");
  Serial.print("Humidity_Clothes(27): "); Serial.println(h1);
  Serial.print("Humidity_Env(26): "); Serial.println(h2);
  Serial.print("Temp_DS18B20: "); Serial.println(t_ds);
  Serial.print("Light: "); Serial.println(lightRaw, DEC);

  // ---- ส่งไป Blynk ----
  Blynk.virtualWrite(V0, t_ds);     // Temperature KY001
  Blynk.virtualWrite(V1, h1);       // Humidity 1
  Blynk.virtualWrite(V2, h2);       // Humidity 2
  Blynk.virtualWrite(V3, lightRaw); // Light RAW
}

void setup() {
  Serial.begin(115200);

  dht1.begin();
  dht2.begin();
  ds18b20.begin();

  analogSetPinAttenuation(LIGHT_AO_PIN, ADC_11db);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(2000L, sendSensor);
}

void loop() {
  Blynk.run();
  timer.run();
}
