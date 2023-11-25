#include <cstdint>
#ifndef ACTIONS_H
#define ACTIONS_H
enum actions : uint8_t {
    MOVE_FORWARD  = 100,
    MOVE_BACKWARD = 101,
    MOVE_RIGHT    = 102,
    MOVE_LEFT     = 103,
    TAKE_FRAME    = 104,
};
#endif