#include "globals.hpp"

const uint8_t motorControlA_pin = 32;
const uint8_t motorControlB_pin = 33;

QueueHandle_t motorQueue;

TaskHandle_t motorTaskHandler;

void setupMotor() {
  pinMode(motorControlA_pin, OUTPUT);
  pinMode(motorControlB_pin, OUTPUT);

  for (int i = 0; i < 2; i++) {
    digitalWrite(motorControlA_pin, HIGH);
    digitalWrite(motorControlB_pin, HIGH);
    delay(500);
    digitalWrite(motorControlA_pin, LOW);
    digitalWrite(motorControlB_pin, LOW);
    delay(500);
  }
  digitalWrite(motorControlA_pin, HIGH);
  digitalWrite(motorControlB_pin, HIGH);

  motorQueue = xQueueCreate(16, sizeof(WantedState));
  xTaskCreatePinnedToCore(motorTask, "motor-task", 8192, NULL, 1, &motorTaskHandler, 0);
}

void motorTask(void *pvParameters) {
  WantedState command;
  WantedState afterCommand;
  for (;;) {
    if (xQueueReceive(motorQueue, &command, portMAX_DELAY) == pdTRUE) {
      if (command == NONE) {
        digitalWrite(motorControlA_pin, HIGH);
        digitalWrite(motorControlB_pin, HIGH);
        motorState = HALT;
      } else if (command == WANT_EXTEND && coverState != EXTENDED) {
        // extend for 8 seconds
        digitalWrite(motorControlA_pin, HIGH);
        digitalWrite(motorControlB_pin, LOW);
        motorState = WORKING;
        vTaskDelay(pdMS_TO_TICKS(8000));
        
        // sends halt command
        xQueueSend(motorQueue, &afterCommand, portMAX_DELAY);
        coverState = EXTENDED;
      } else if (command == WANT_RETRACT && coverState != RETRACTED) {
        // retract for 8 seconds
        digitalWrite(motorControlA_pin, LOW);
        digitalWrite(motorControlB_pin, HIGH);
        vTaskDelay(pdMS_TO_TICKS(8000));
        motorState = WORKING;

        // sends halt command
        xQueueSend(motorQueue, &afterCommand, portMAX_DELAY);
        coverState = RETRACTED;
      } else {
        Serial.println("Unknown command received from motorQueue.");
      }
    } else {
      Serial.println("motorQueue receive error.");
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}