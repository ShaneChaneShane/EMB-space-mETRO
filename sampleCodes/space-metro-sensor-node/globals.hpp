#ifndef GLOBALS_HPP
#define GLOBALS_HPP

// all possible state of plastic cover
enum CoverState {
  UNKNOWN,
  RETRACTED,
  MOVING_EXTEND,
  EXTENDED,
  MOVING_RETRACT
};

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
  LIMIT_EXTEND_HIT,
  LIMIT_RETRACT_HIT,
  CMD_EXTEND,
  CMD_RETRACT
};

extern CoverState coverState;

// queues
extern QueueHandle_t eventQueue;
extern QueueHandle_t motorQueue;

#endif