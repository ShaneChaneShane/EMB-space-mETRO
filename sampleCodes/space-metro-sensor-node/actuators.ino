#include "globals.hpp"

const uint8_t motorControlA_pin = 32;
const uint8_t motorControlB_pin = 33;

QueueHandle_t motorQueue;

TaskHandle_t motorTaskHandler;

void setupMotor() {
  pinMode(motorControlA_pin, OUTPUT);
  pinMode(motorControlB_pin, OUTPUT);

  motorQueue = xQueueCreate(16, sizeof(WantedState));
  xTaskCreatePinnedToCore(motorTask, "motor-task", 8192, NULL, 2, &motorTaskHandler, 1);
}

void motorTask(void *pvParameters) {
  WantedState command;
  for (;;) {
    if (xQueueReceive(motorQueue, &command, portMAX_DELAY) == pdTRUE) {
      if (command == NONE) {
        digitalWrite(motorControlA_pin, HIGH);
        digitalWrite(motorControlB_pin, HIGH);
      } else if (command == WANT_EXTEND && coverState != EXTENDED) {
        digitalWrite(motorControlA_pin, HIGH);
        digitalWrite(motorControlB_pin, LOW);
        coverState = MOVING_EXTEND;
      } else if (command == WANT_RETRACT && coverState != RETRACTED) {
        digitalWrite(motorControlA_pin, LOW);
        digitalWrite(motorControlB_pin, HIGH);
        coverState = MOVING_RETRACT;
      } else {
        Serial.println("Unknown command received from motorQueue.");
      }
    } else {
      Serial.println("motorQueue receive error.");
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}