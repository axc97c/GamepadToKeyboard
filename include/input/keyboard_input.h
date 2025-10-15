#ifndef KEYBOARD_INPUT_H
#define KEYBOARD_INPUT_H

#include <Arduino.h>
#include <USBHost_t36.h>

class KeyboardInput
{
private:
    KeyboardController *keyboard;

    // Track last key press to avoid double-reading
    int lastKeyCode;
    unsigned long lastKeyTime;
    const unsigned long keyDebounceDelay = 200; // ms

public:
    KeyboardInput(KeyboardController *kbd);

    void setup();
    void reset();

    // Check if a new key has been pressed
    // Returns the key code if pressed, or 0 if no key pressed
    int getKeyPress();

    // Check if keyboard is available
    bool isAvailable();
};

#endif // KEYBOARD_INPUT_H
