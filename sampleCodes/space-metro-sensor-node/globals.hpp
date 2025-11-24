#ifndef GLOBALS_HPP
#define GLOBALS_HPP

// enum for sending out to other nodes
enum CoverState {
  RETRACTED,
  EXTENDED,
  UNKNOWN
};

enum MotorState {
  HALT,
  WORKING
};

enum RainingState {
  DRY,
  RAINING
}

// where we want plastic cover to be now
enum WantedState {
  NONE,
  WANT_RETRACT,
  WANT_EXTEND
};

// outside factors/sensors/commands
enum EventType {
  RAIN_ON,
  RAIN_OFF,
  CMD_EXTEND,
  CMD_RETRACT
};

extern CoverState coverState;
extern MotorState motorState;
extern RainingState rainingState;

// queues
extern QueueHandle_t eventQueue;
extern QueueHandle_t motorQueue;

#endif