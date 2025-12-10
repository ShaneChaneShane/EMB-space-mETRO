#include "globals.hpp"

const uint8_t raindropSensor_pin = 25;

TaskHandle_t rainSensorTask;

void setupSensors() {
  pinMode(raindropSensor_pin, INPUT);
  xTaskCreatePinnedToCore(readRaindrop, "read-raindrop-task", 8192, NULL, 1, &rainSensorTask, 0);
}

// entry for external commands
void readCommand() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    EventType event = input.startsWith("f") ? CMD_EXTEND : CMD_RETRACT;
    xQueueSend(eventQueue, &event, portMAX_DELAY);
  }
}

// raindrop sensor & rain state updates
void readRaindrop(void *pvParameters) {
  EventType lastEvent = (digitalRead(raindropSensor_pin)) ? RAIN_ON : RAIN_OFF;
  for (;;) {
    bool isRaining = (bool) digitalRead(raindropSensor_pin);
    rainingState = isRaining ? RAINING : DRY;
    EventType event = isRaining ? RAIN_ON : RAIN_OFF;
    if (event != lastEvent) {
      xQueueSend(eventQueue, &event, portMAX_DELAY); // only send if state change
      lastEvent = event;
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
