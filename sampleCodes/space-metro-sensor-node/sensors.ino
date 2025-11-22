#include "globals.hpp"

const uint8_t limExtendButton_pin = 34;
const uint8_t limRetractButton_pin = 35;
const uint8_t raindropSensor_pin = 25;

TaskHandle_t rainSensorTask;

void setupSensors() {
  pinMode(limExtendButton_pin, INPUT);
  pinMode(limRetractButton_pin, INPUT);
  pinMode(raindropSensor_pin, INPUT);

  xTaskCreatePinnedToCore(readRaindrop, "read-raindrop-task", 8192, NULL, 2, &rainSensorTask, 0);
}

unsigned long debounceDelay = 50;
unsigned long lastDebounceTime[2] = { 0, 0 };
bool lastButtonState[2] = { true, true };
bool buttonState[2] = { true, true };

void readButtons() {
  EventType event1, event2;
  bool reading[2] = { (bool)digitalRead(limExtendButton_pin), (bool)digitalRead(limRetractButton_pin) };

  // reading[0]
  if (reading[0] != lastButtonState[0]) {
    lastDebounceTime[0] = millis();
  }
  if ((millis() - lastDebounceTime[0]) > debounceDelay) {
    if (reading[0] != buttonState[0]) {
      buttonState[0] = reading[0];
      // Serial.println(buttonState[0]);
      if (!buttonState[0]) {
        Serial.println("Debounced triggered! (limExtendButton)");
        event1 = LIMIT_EXTEND_HIT;
        if (xQueueSend(eventQueue, &event1, portMAX_DELAY) != pdTRUE) {
          Serial.println("Failed to send to queue via limExtendButton");
        }
      }
    }
  }
  lastButtonState[0] = reading[0];

  // reading[1]
  if (reading[1] != lastButtonState[1]) {
    lastDebounceTime[1] = millis();
  }
  if ((millis() - lastDebounceTime[1]) > debounceDelay) {
    if (reading[1] != buttonState[1]) {
      buttonState[1] = reading[1];
      // Serial.println(buttonState[1]);
      if (!buttonState[1]) {
        Serial.println("Debounced triggered! (limRetractButton)");
        event2 = LIMIT_RETRACT_HIT;
        if (xQueueSend(eventQueue, &event2, portMAX_DELAY) != pdTRUE) {
          Serial.println("Failed to send to queue via limRetractButton");
        }
      }
    }
  }
  lastButtonState[1] = reading[1];
}

void readCommand() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    EventType event = input.startsWith("f") ? CMD_EXTEND : CMD_RETRACT;
    xQueueSend(eventQueue, &event, portMAX_DELAY);
  }
}

void readRaindrop(void *pvParameters) {
  for (;;) {
    EventType event = (digitalRead(raindropSensor_pin)) ? RAIN_ON : RAIN_OFF;
    xQueueSend(eventQueue, &event, portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
