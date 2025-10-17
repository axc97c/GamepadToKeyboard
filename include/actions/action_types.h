#ifndef ACTION_TYPE_H
#define ACTION_TYPE_H

#include <Arduino.h>
#include <cstdint>
#include "utils.h"

enum class ActionType : uint8_t
{
    NONE,
    RUN,
    MENU,
    BIND_KEY
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
    ARROW_KEYS,       // Emulates arrow keys
    JOYSTICK,         // Both joystick axis
    JOYSTICK_X,       // Horizontal joystick axis only
    JOYSTICK_Y        // Vertical joystick axis only
};

enum class TriggerBehavior : uint8_t
{
    DISABLED,
    MOUSE_X,
    MOUSE_Y,
    SCROLL_WHEEL,
    BUTTONS,
    JOYSTICK_X,
    JOYSTICK_Y
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
    float sensitivity = 0.15f;
    int deadzone = 16;
    int activationThreshold = 64;

    int keyUp = KEY_UP;
    int keyDown = KEY_DOWN;
    int keyLeft = KEY_LEFT;
    int keyRight = KEY_RIGHT;

    bool upPressed = false;
    bool downPressed = false;
    bool leftPressed = false;
    bool rightPressed = false;
};

struct TriggerConfig
{
    TriggerBehavior behavior = TriggerBehavior::DISABLED;
    float sensitivity = 0.15f;
    int deadzone = 16;
    int activationThreshold = 64;

    int keyLeft = KEY_LEFT;
    int keyRight = KEY_RIGHT;

    bool leftPressed = false;
    bool rightPressed = false;
};

// Complete joystick mapping configuration
struct JoystickMappingConfig
{
    static const int MAX_MAPPINGS = 32;
    static const int MAX_FILENAME_LENGTH = 64;

    char filename[MAX_FILENAME_LENGTH];
    char displayName[MAX_FILENAME_LENGTH];
    ButtonMapping mappings[MAX_MAPPINGS];
    int numMappings;
    StickConfig leftStick;
    StickConfig rightStick;
    TriggerConfig triggers;
    bool modified; // Flag to track if config has been changed since loading

    JoystickMappingConfig() : numMappings(0), modified(false)
    {
        filename[0] = '\0';
    }

    void setFilename(const char *newName)
    {
        strncpy(filename, newName, MAX_FILENAME_LENGTH - 1);
        filename[MAX_FILENAME_LENGTH - 1] = '\0';

        Utils::trimFilenameToBuffer(filename, displayName, MAX_FILENAME_LENGTH);

        Serial.print("Setting filename to: ");
        Serial.println(newName);
        Serial.print("Saved filename as: ");
        Serial.println(filename);
        Serial.print("Display name as: ");
        Serial.println(displayName);
    }
};

struct RunActionParams
{
    char filename[JoystickMappingConfig::MAX_FILENAME_LENGTH];
};

struct StickConfigActionParams
{
    bool isRight;
};

enum class BindKeyTarget : uint8_t
{
    BUTTON_MAPPING,  // Binding a button from mappings array
    TRIGGER_LEFT,    // Binding left trigger key
    TRIGGER_RIGHT,   // Binding right trigger key
    STICK_UP,        // Binding stick up direction key
    STICK_DOWN,      // Binding stick down direction key
    STICK_LEFT,      // Binding stick left direction key
    STICK_RIGHT      // Binding stick right direction key
};

struct BindKeyActionParams
{
    BindKeyTarget target;
    int mappingIndex; // Index into mappingConfig.mappings array (only used for BUTTON_MAPPING)
    bool isRightStick; // true for right stick, false for left stick (only used for STICK_* targets)
};

// Global joystick mapping configuration instance
extern JoystickMappingConfig mappingConfig;

#endif