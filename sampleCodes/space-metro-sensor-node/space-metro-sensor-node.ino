#include "globals.hpp"

TaskHandle_t controlTaskHandler;
TaskHandle_t sendStateTaskHandler;
QueueHandle_t eventQueue;

CoverState coverState = UNKNOWN;
MotorState motorState = HALT;
bool isRaining = false;

// entry point for sending out parameters
void sendStateTask(void *pvParameters) {
  for (;;) {
    Serial.printf("MotorState: %d\n", motorState);
    Serial.printf("CoverState: %d\n", coverState);
    Serial.printf("RainingState: %d\n", rainingState);
    Serial.println("--------------------");
    vTaskDelay(pdMS_TO_TICKS(1000)); // send every x milliseconds
  }
}

// controlling motor on event received
void controlTask(void *pvParameters) {
  EventType e;
  WantedState wantedState;
  for (;;) {
    if (xQueueReceive(eventQueue, &e, portMAX_DELAY) == pdTRUE) {
      switch (e) {
        case RAIN_ON:
          Serial.println("RAIN_ON");
          if (coverState != EXTENDED) {
            wantedState = WANT_EXTEND;
            if (xQueueSend(motorQueue, &wantedState, portMAX_DELAY) != pdTRUE) {
              Serial.println("Failed to send to queue via activation signal");
            }
          }
          break;
        case RAIN_OFF:
          Serial.println("RAIN_OFF");
          if (coverState != RETRACTED) {
            wantedState = WANT_RETRACT;
            if (xQueueSend(motorQueue, &wantedState, portMAX_DELAY) != pdTRUE) {
              Serial.println("Failed to send to queue via deactivation signal");
            }
          }
          break;
        case CMD_EXTEND:
          Serial.println("CMD_EXTEND");
          if (coverState != EXTENDED) {
            wantedState = WANT_EXTEND;
            if (xQueueSend(motorQueue, &wantedState, portMAX_DELAY) != pdTRUE) {
              Serial.println("Failed to send to queue via activation signal");
            }
          }
          break;
        case CMD_RETRACT:
          Serial.println("CMD_RETRACT");
          if (coverState != RETRACTED) {
            wantedState = WANT_RETRACT;
            if (xQueueSend(motorQueue, &wantedState, portMAX_DELAY) != pdTRUE) {
              Serial.println("Failed to send to queue via deactivation signal");
            }
          }
          break;
        default:
          Serial.println("Unknown event");
          wantedState = NONE;
          break;
      }
    } else {
      Serial.println("eventQueue receive error.");
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void setup() {
  Serial.begin(115200);

  // setups
  setupMotor();
  setupSensors();

  // queue creation
  eventQueue = xQueueCreate(16, sizeof(EventType));
  xTaskCreatePinnedToCore(controlTask, "main-control-task", 8192, NULL, 2, &controlTaskHandler, 0);
  xTaskCreatePinnedToCore(sendStateTask, "send-state-task", 8192, NULL, 2, &sendStateTaskHandler, 1);
}

void loop() {
  // put your main code here, to run repeatedly:
  readCommand();
}
