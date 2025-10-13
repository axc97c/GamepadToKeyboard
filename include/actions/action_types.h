#ifndef ACTION_TYPE_H
#define ACTION_TYPE_H

#include <Arduino.h>
#include <cstdint>

struct ButtonMapping {
  uint8_t joystickButton;  // Button number on joystick
  int keyCode;             // Keyboard key code to press
  bool currentlyPressed;   // Track state to avoid repeat triggers
};

enum class ActionType : uint8_t {
  NONE,
  RUN,
  MENU
};


struct RunActionParams
{
    uint8_t profileId;
};

#endif