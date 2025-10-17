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
    float sensitivity = 0.15f;
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
    bool modified; // Flag to track if config has been changed since loading

    JoystickMappingConfig() : numMappings(0), modified(false)
    {
        filename[0] = '\0';
    }

    void setFilename(const char *newName)
    {
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wstringop-truncation"
        strncpy(filename, newName, MAX_FILENAME_LENGTH - 1);
        #pragma GCC diagnostic pop
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

struct MenuActionParams
{
    uint8_t menuId;
};

struct StickConfigActionParams  : MenuActionParams
{
    bool isRight;
};

struct BindKeyActionParams
{
    int mappingIndex; // Index into mappingConfig.mappings array
};

// Global joystick mapping configuration instance
extern JoystickMappingConfig mappingConfig;

#endif