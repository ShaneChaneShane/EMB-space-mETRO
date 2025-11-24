#include "globals.hpp"

TaskHandle_t controlTaskHandler;
QueueHandle_t eventQueue;

CoverState coverState = UNKNOWN;

void controlTask(void *pvParameters) {
  EventType e;
  WantedState wantedState;
  for (;;) {
    if (xQueueReceive(eventQueue, &e, portMAX_DELAY) == pdTRUE) {
      // debug
      switch (e) {
        case RAIN_ON:
          Serial.println("RAIN_ON");
          break;
        case RAIN_OFF:
          Serial.println("RAIN_OFF");
          break;
        case LIMIT_EXTEND_HIT:
          Serial.println("LIMIT_EXTEND_HIT");
          break;
        case LIMIT_RETRACT_HIT:
          Serial.println("LIMIT_RETRACT_HIT");
          break;
        case CMD_EXTEND:
          Serial.println("CMD_EXTEND");
          break;
        case CMD_RETRACT:
          Serial.println("CMD_RETRACT");
          break;
        default:
          Serial.println("Unknown event");
          break;
      }

      if (e == LIMIT_EXTEND_HIT || e == LIMIT_RETRACT_HIT) {
        wantedState = NONE;
        if (xQueueSend(motorQueue, &wantedState, portMAX_DELAY) != pdTRUE) {
          Serial.println("Failed to send to queue via limit switches hit");
        }
        coverState = (e == LIMIT_EXTEND_HIT) ? EXTENDED : RETRACTED;
      } 
      
      else if (e == RAIN_ON || e == CMD_EXTEND) {
        if (coverState != EXTENDED) {
          wantedState = WANT_EXTEND;
          if (xQueueSend(motorQueue, &wantedState, portMAX_DELAY) != pdTRUE) {
            Serial.println("Failed to send to queue via activation signal");
          }
          coverState = MOVING_EXTEND;
        }
      } 
      
      else if (e == RAIN_OFF || e == CMD_RETRACT) {
        if (coverState != RETRACTED) {
          wantedState = WANT_RETRACT;
          if (xQueueSend(motorQueue, &wantedState, portMAX_DELAY) != pdTRUE) {
            Serial.println("Failed to send to queue via deactivation signal");
          }
          coverState = MOVING_RETRACT;
        }
      } 
      
      else wantedState = NONE;

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
  xTaskCreatePinnedToCore(controlTask, "main-control-task", 8192, NULL, 1, &controlTaskHandler, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  readButtons();
  readCommand();
}
