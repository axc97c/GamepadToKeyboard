#ifndef JOYSTICK_MAPPINGS_H
#define JOYSTICK_MAPPINGS_H

#include <Arduino.h>
#include <USBHost_t36.h>

// Generic button definitions (based on Xbox layout)
namespace GenericController
{
    // Face buttons (right side, diamond layout)
    const uint8_t BTN_SOUTH = 0; // A / Cross (bottom)
    const uint8_t BTN_EAST = 1;  // B / Circle (right)
    const uint8_t BTN_WEST = 2;  // X / Square (left)
    const uint8_t BTN_NORTH = 3; // Y / Triangle (top)

    // Shoulder buttons
    const uint8_t BTN_L1 = 4; // LB / L1
    const uint8_t BTN_R1 = 5; // RB / R1
    const uint8_t BTN_L2 = 6; // LT button / L2
    const uint8_t BTN_R2 = 7; // RT button / R2

    // Center buttons
    const uint8_t BTN_SELECT = 8; // Back / Share
    const uint8_t BTN_START = 9;  // Start / Options
    const uint8_t BTN_MENU = 10;  // Xbox / PS button

    // Stick clicks
    const uint8_t BTN_L3 = 11; // Left stick click
    const uint8_t BTN_R3 = 12; // Right stick click

    // D-Pad buttons
    const uint8_t BTN_DPAD_UP = 13;
    const uint8_t BTN_DPAD_DOWN = 14;
    const uint8_t BTN_DPAD_LEFT = 15;
    const uint8_t BTN_DPAD_RIGHT = 16;

    // Special
    const uint8_t BTN_TOUCHPAD = 17; // PS4 touchpad

    // Analog Axes
    const uint8_t AXIS_LEFT_X = 0;
    const uint8_t AXIS_LEFT_Y = 1;
    const uint8_t AXIS_RIGHT_X = 2;
    const uint8_t AXIS_RIGHT_Y = 3;
    const uint8_t AXIS_LEFT_TRIGGER = 4;
    const uint8_t AXIS_RIGHT_TRIGGER = 5;
}

// Mapper class to translate controller-specific buttons to generic buttons
class JoystickMapping
{
private:
    // Button name mapping
    struct ButtonNameMapping
    {
        uint8_t button;
        const char *name;
    };
    
    static const ButtonNameMapping buttonNameMap[];
    static const int buttonNameMapSize;

public:
    // Map controller-specific button to generic button
    static int mapButtonToGeneric(JoystickController::joytype_t type, uint8_t controllerButton);

    // Map controller-specific axis to generic axis
    static int mapAxisToGeneric(JoystickController::joytype_t type, uint8_t controllerAxis);

    // Map generic button to controller-specific button (reverse mapping)
    static int mapGenericToButton(JoystickController::joytype_t type, uint8_t genericButton);

    // Check if controller uses D-pad as axis and get the axis number
    static bool usesDPadAxis(JoystickController::joytype_t type, uint8_t &axisNumber);

    // Map D-pad axis value to generic D-pad button
    static int mapDPadValueToButton(JoystickController::joytype_t type, uint8_t axisValue);

    // Get button name for generic button
    static const char *getGenericButtonName(uint8_t genericButton);

    // Parse button name to generic button
    static int parseGenericButtonName(const char *buttonName);
};

// Xbox 360 physical button mapping
namespace Xbox360Physical
{
    const uint8_t A = 4;
    const uint8_t B = 5;
    const uint8_t X = 6;
    const uint8_t Y = 7;
    const uint8_t LB = 0;
    const uint8_t RB = 1;
    const uint8_t BACK = 13;
    const uint8_t START = 12;
    const uint8_t XBOX_BUTTON = 2;
    const uint8_t LEFT_STICK = 14;
    const uint8_t RIGHT_STICK = 15;
    const uint8_t DPAD_UP = 8;
    const uint8_t DPAD_DOWN = 9;
    const uint8_t DPAD_LEFT = 10;
    const uint8_t DPAD_RIGHT = 11;
}

// PS4 physical button mapping
namespace PS4Physical
{
    const uint8_t CROSS = 1;
    const uint8_t CIRCLE = 2;
    const uint8_t SQUARE = 0;
    const uint8_t TRIANGLE = 3;
    const uint8_t L1 = 4;
    const uint8_t R1 = 5;
    const uint8_t L2 = 6;
    const uint8_t R2 = 7;
    const uint8_t SHARE = 8;
    const uint8_t OPTIONS = 9;
    const uint8_t PS_BUTTON = 12;
    const uint8_t L3 = 10;
    const uint8_t R3 = 11;
    const uint8_t TOUCHPAD = 13;

    // D-Pad as axis
    const uint8_t DPAD_AXIS = 9;
    const uint8_t DPAD_UP_VALUE = 0;
    const uint8_t DPAD_UP_RIGHT_VALUE = 1;
    const uint8_t DPAD_RIGHT_VALUE = 2;
    const uint8_t DPAD_DOWN_RIGHT_VALUE = 3;
    const uint8_t DPAD_DOWN_VALUE = 4;
    const uint8_t DPAD_DOWN_LEFT_VALUE = 5;
    const uint8_t DPAD_LEFT_VALUE = 6;
    const uint8_t DPAD_UP_LEFT_VALUE = 7;
    const uint8_t DPAD_NEUTRAL_VALUE = 8;
}

#endif