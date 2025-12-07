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
  if (motorQueue == NULL) {
    Serial.println("Failed to create motorQueue!");
    while (true) { delay(1000); }
  }

  xTaskCreatePinnedToCore(motorTask, "motor-task", 8192, NULL, 1, &motorTaskHandler, 0);
}

void motorTask(void *pvParameters) {
  WantedState command;

  for (;;) {
    if (xQueueReceive(motorQueue, &command, portMAX_DELAY) == pdTRUE) {
      if (command == NONE) {
        digitalWrite(motorControlA_pin, HIGH);
        digitalWrite(motorControlB_pin, HIGH);
        motorState = HALT;
      } 
      else if (command == WANT_EXTEND && coverState != EXTENDED) {
        Serial.println("MOTOR: EXTEND");
        digitalWrite(motorControlA_pin, HIGH);
        digitalWrite(motorControlB_pin, LOW);
        motorState = WORKING;
        vTaskDelay(pdMS_TO_TICKS(8000));

        
        WantedState afterCommand = NONE;
        xQueueSend(motorQueue, &afterCommand, portMAX_DELAY);

        coverState = EXTENDED;
      } 
      else if (command == WANT_RETRACT && coverState != RETRACTED) {
        Serial.println("MOTOR: RETRACT");
        digitalWrite(motorControlA_pin, LOW);
        digitalWrite(motorControlB_pin, HIGH);
        motorState = WORKING;
        vTaskDelay(pdMS_TO_TICKS(8000));

        WantedState afterCommand = NONE;
        xQueueSend(motorQueue, &afterCommand, portMAX_DELAY);

        coverState = RETRACTED;
      } 
      else {
        Serial.println("Unknown command received from motorQueue.");
      }
    } else {
      Serial.println("motorQueue receive error.");
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}
