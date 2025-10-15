#ifndef KEYBOARD_INPUT_H
#define KEYBOARD_INPUT_H

#include <Arduino.h>
#include <USBHost_t36.h>

class KeyboardInput
{
private:
    KeyboardController *keyboard;

    // Track pending key press from callback
    static int pendingKeyCode;
    static unsigned long pendingKeyTime;

    // Callback handlers (must be static)
    static void onKeyPress(int unicode);
    static void onKeyRelease(int unicode);

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
