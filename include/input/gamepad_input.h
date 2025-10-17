#ifndef INPUT_H
#define INPUT_H

#include <Arduino.h>
#include <USBHost_t36.h>
#include "mapping/joystick_mappings.h"

// Input event types
enum GamepadInputEvent
{
    INPUT_NONE = 0,
    INPUT_UP,
    INPUT_DOWN,
    INPUT_LEFT,
    INPUT_RIGHT,
    INPUT_CONFIRM, // Primary action button (A/Cross)
    INPUT_CANCEL,  // Secondary action button (B/Circle)
    INPUT_MENU,    // Menu button
    INPUT_START,   // Start/Options button
    INPUT_SELECT   // Select/Share button
};

class GamepadInput
{
private:
    JoystickController *joystick;

    // Button state tracking
    uint32_t lastButtons;
    uint8_t lastDPadValue;

    // Timing for debounce and repeat
    unsigned long buttonPressTime[32]; // Track press time for each button
    bool buttonRepeating[32];          // Track if button is in repeat mode

    const unsigned long debounceDelay = 150;  // Initial delay before registering press
    const unsigned long repeatDelay = 400;    // Delay before repeat starts
    const unsigned long repeatInterval = 100; // Interval between repeats

    // Helper methods
    bool isButtonPressed(uint8_t genericButton);
    bool isDPadPressed(uint8_t dpadButton);
    GamepadInputEvent checkButton(uint8_t genericButton, GamepadInputEvent event);
    GamepadInputEvent checkDPad();

public:
    GamepadInput(JoystickController *controller);

    void reset();

    // Get the current input event (with debouncing and repeat)
    GamepadInputEvent getEvent();

    // Check if a specific button is currently held (no debounce)
    bool isHeld(uint8_t genericButton);
};

#endif // INPUT_H