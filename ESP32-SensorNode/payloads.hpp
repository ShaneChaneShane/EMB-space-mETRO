#ifndef PAYLOADS_HPP
#define PAYLOADS_HPP

#include "globals.hpp"

struct SensorPayload {
  float humidity1;
  float humidity2;
  float temperature;
  uint16_t light;
  bool isRaining;
};

struct CommandPayload {
  bool openCover;
};

struct StatePayload {
  CoverState coverState;
  MotorState motorState;
  RainingState rainingState;
};

struct PacketHeader {
  BoardId src;
  BoardId dst;
  MessageType type;
  uint16_t seq;
};

struct Packet {
  PacketHeader header;
  union {
    CommandPayload cmd;
    SensorPayload sensor;
    StatePayload state;
  } payload;
} __attribute__((packed)) ;

#endif