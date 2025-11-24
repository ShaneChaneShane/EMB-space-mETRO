#ifndef GLOBALS_HPP
#define GLOBALS_HPP

// enum for sending out to other nodes
enum CoverState {
  UNKNOWN,
  RETRACTED,
  EXTENDED
};

enum MotorState {
  HALT,
  WORKING
};

enum RainingState {
  DRY,
  RAINING
};

// where we want plastic cover to be now
enum WantedState {
  NONE,
  WANT_RETRACT,
  WANT_EXTEND
};

enum BoardId : uint8_t {
  MINT = 0,
  BRAIN = 1, 
  BEAU = 2
};

// outside factors/sensors/commands
enum EventType {
  RAIN_ON,
  RAIN_OFF,
  CMD_EXTEND,
  CMD_RETRACT
};

enum MessageType {
  MSG_ACK = 0,
  MSG_CMD = 1,
  MSG_STATE = 2,
  MSG_SENSOR = 3
};

extern CoverState coverState;
extern MotorState motorState;
extern RainingState rainingState;

// queues
extern QueueHandle_t eventQueue;
extern QueueHandle_t motorQueue;
extern QueueHandle_t espNowQueue;

#endif