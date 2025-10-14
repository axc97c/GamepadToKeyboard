#ifndef ACTION_TYPE_H
#define ACTION_TYPE_H

#include <Arduino.h>
#include <cstdint>

enum class ActionType : uint8_t
{
    NONE,
    RUN,
    MENU
};

enum class MappingType : uint8_t
{
    GENERIC_BUTTON // All buttons now use generic mapping
};

// Analog stick behavior types
enum class StickBehavior : uint8_t
{
    DISABLED,         // Stick does nothing
    MOUSE_MOVEMENT,   // Controls mouse X/Y movement
    BUTTON_EMULATION, // Acts as 4 buttons (up/down/left/right)
    SCROLL_WHEEL,     // Y axis controls scroll wheel
    WASD_KEYS,        // Emulates WASD keys
    ARROW_KEYS        // Emulates arrow keys
};

// Analog stick identifier
enum class StickId : uint8_t
{
    LEFT_STICK = 0,
    RIGHT_STICK = 1
};

struct ButtonMapping
{
    uint8_t genericButton;
    int keyCode;
    bool currentlyPressed;
};

// Analog stick configuration
struct StickConfig
{
    StickBehavior behavior = StickBehavior::DISABLED;
    float sensitivity = 0.015f;
    int deadzone = 16;
    int activationThreshold = 64;

    // For button emulation and custom keys
    int keyUp = KEY_UP;
    int keyDown = KEY_DOWN;
    int keyLeft = KEY_LEFT;
    int keyRight = KEY_RIGHT;

    // State tracking for button emulation
    bool upPressed = false;
    bool downPressed = false;
    bool leftPressed = false;
    bool rightPressed = false;
};

struct RunActionParams
{
    uint8_t profileId;
};

struct MenuActionParams
{
    uint8_t menuId;
};

#endif