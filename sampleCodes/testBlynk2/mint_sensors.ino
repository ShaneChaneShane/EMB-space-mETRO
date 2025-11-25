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

void setupSensorsMint() {
  dht1.begin();
  dht2.begin();
  ds18b20.begin();

  pinMode(LIGHT_AO_PIN, INPUT);
}

//read every 1 sec
void readSensorsTask(void *pvParameters) {
  for (;;) {
    float h1 = dht1.readHumidity();
    float h2 = dht2.readHumidity();

    ds18b20.requestTemperatures();
    float t_ds = ds18b20.getTempCByIndex(0);

    int lightRaw = analogRead(LIGHT_AO_PIN);

    // not Nan
    if (!isnan(h1)) {
      Blynk.virtualWrite(V0, h1);
    }
    if (!isnan(h2)) {
      Blynk.virtualWrite(V1, h2);
    }
    if (!isnan(t_ds)) {
      Blynk.virtualWrite(V2, t_ds);
    }

    Blynk.virtualWrite(V3, lightRaw);

    // stage brain to blynk
    Blynk.virtualWrite(V4, (int)coverState);
    Blynk.virtualWrite(V5, (int)motorState);
    Blynk.virtualWrite(V6, (int)rainingState);

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
