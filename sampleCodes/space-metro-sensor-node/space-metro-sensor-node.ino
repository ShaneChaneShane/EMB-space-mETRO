#include "globals.hpp"
#include "payloads.hpp"
#include <esp_now.h>
#include <WiFi.h>
#include "firebase.hpp"

TaskHandle_t controlTaskHandler;
TaskHandle_t sendStateTaskHandler;
QueueHandle_t eventQueue;

CoverState coverState = UNKNOWN;
MotorState motorState = HALT;
RainingState rainingState = DRY;

// entry point for sending out parameters
void sendStateTask(void *pvParameters) {
  Packet p;
  for (;;) {
    Serial.printf("MotorState: %d\n", motorState);
    Serial.printf("CoverState: %d\n", coverState);
    Serial.printf("RainingState: %d\n", rainingState);
    Serial.println("--------------------");

    p.header.dst = MINT;
    p.header.type = MSG_STATE;
    p.payload.state.motorState = motorState;
    p.payload.state.coverState = coverState;
    p.payload.state.rainingState = rainingState;
    if (xQueueSend(espNowQueue, &p, portMAX_DELAY) != pdTRUE) {
      Serial.println("Failed to send to esp-now queue");
    }
    vTaskDelay(pdMS_TO_TICKS(1000)); // send every x milliseconds
  }
}
// ===== ESP-NOW receive callback (จาก Beau) =====
// void OnDataRecv(const esp_now_recv_info * info, const uint8_t *data, int len) {
//   if (len <= 0 || data == nullptr) return;

//   // แปลง data เป็นสตริง
//   String msg = String((const char *)data);
//   msg.trim();

//   Serial.print("ESP-NOW recv: ");
//   Serial.println(msg);

//   EventType e;

//   if (msg == "CMD_EXTEND") {
//     e = CMD_EXTEND;
//   } else if (msg == "CMD_RETRACT") {
//     e = CMD_RETRACT;
//   } else {
//     Serial.println("Unknown ESP-NOW message, ignore");
//     return;
//   }

//   if (eventQueue != nullptr) {
//     if (xQueueSend(eventQueue, &e, 0) != pdTRUE) {
//       Serial.println("Failed to enqueue event from ESP-NOW");
//     }
//   }
// }

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
  eventQueue = xQueueCreate(16, sizeof(EventType));
  if (eventQueue == nullptr) {
    Serial.println("Failed to create eventQueue");
  }
  // setups
  setupMotor();
  setupSensors();
  setupEspNow();

  // queue creation
  eventQueue = xQueueCreate(16, sizeof(EventType));
  xTaskCreatePinnedToCore(controlTask, "main-control-task", 8192, NULL, 2, &controlTaskHandler, 0);
  xTaskCreatePinnedToCore(sendStateTask, "send-state-task", 8192, NULL, 2, &sendStateTaskHandler, 1);
  Serial.println("Setup Complete!");
}

void loop() {
  // put your main code here, to run repeatedly:
  readCommand();
}
