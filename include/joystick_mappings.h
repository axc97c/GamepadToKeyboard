#ifndef JOYSTICK_MAPPINGS_H
#define JOYSTICK_MAPPINGS_H

#include <Arduino.h>

// Xbox 360 Controller Button Mappings
namespace Xbox360 {
    // Buttons
    const uint8_t LB = 0;
    const uint8_t RB = 1;
    const uint8_t XBOX_BUTTON = 2;
    const uint8_t UNKNOWN = 3;
    const uint8_t A = 4;
    const uint8_t B = 5;
    const uint8_t X = 6;
    const uint8_t Y = 7;
    const uint8_t DPAD_UP = 8;
    const uint8_t DPAD_DOWN = 9;
    const uint8_t DPAD_LEFT = 10;
    const uint8_t DPAD_RIGHT = 11;
    const uint8_t START = 12;
    const uint8_t BACK = 13;
    const uint8_t LEFT_STICK = 14;
    const uint8_t RIGHT_STICK = 15;
 
    // Analog Axes
    const uint8_t AXIS_LEFT_X = 0;
    const uint8_t AXIS_LEFT_Y = 1;
    const uint8_t AXIS_RIGHT_X = 2;
    const uint8_t AXIS_RIGHT_Y = 3;
    const uint8_t AXIS_LEFT_TRIGGER = 6;
    const uint8_t AXIS_RIGHT_TRIGGER = 5;
}

#endif