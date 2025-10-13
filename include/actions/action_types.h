#ifndef ACTION_TYPE_H
#define ACTION_TYPE_H

#include <Arduino.h>
#include <cstdint>

enum class ActionType : uint8_t {
  NONE,
  RUN,
  MENU
};

enum class MappingType : uint8_t {
    GENERIC_BUTTON  // All buttons now use generic mapping
};

// Analog stick behavior types
enum class StickBehavior : uint8_t {
    DISABLED,           // Stick does nothing
    MOUSE_MOVEMENT,     // Controls mouse X/Y movement
    BUTTON_EMULATION,   // Acts as 4 buttons (up/down/left/right)
    SCROLL_WHEEL,       // Y axis controls scroll wheel
    WASD_KEYS,          // Emulates WASD keys
    ARROW_KEYS,         // Emulates arrow keys
    CUSTOM_KEYS         // User-defined keys for each direction
};

// Analog stick identifier
enum class StickId : uint8_t {
    LEFT_STICK = 0,
    RIGHT_STICK = 1
};

struct ButtonMapping {
    uint8_t genericButton;       // Generic button number
    int keyCode;                 // Keyboard key code to press
    bool currentlyPressed;       // Track state
};

// Analog stick configuration
struct StickConfig {
    StickBehavior behavior;      // What this stick does
    float sensitivity;           // Movement sensitivity (for mouse/scroll)
    int deadzone;                // Deadzone threshold
    int activationThreshold;     // Threshold for button emulation
    
    // For button emulation and custom keys
    int keyUp;                   // Key for up/positive Y
    int keyDown;                 // Key for down/negative Y
    int keyLeft;                 // Key for left/negative X
    int keyRight;                // Key for right/positive X
    
    // State tracking for button emulation
    bool upPressed;
    bool downPressed;
    bool leftPressed;
    bool rightPressed;
};

struct RunActionParams
{
    uint8_t profileId;
};

#endif