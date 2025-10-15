#include "input/keyboard_input.h"

KeyboardInput::KeyboardInput(KeyboardController *kbd)
    : keyboard(kbd), lastKeyCode(0), lastKeyTime(0)
{
}

void KeyboardInput::setup()
{
    reset();
}

void KeyboardInput::reset()
{
    lastKeyCode = 0;
    lastKeyTime = 0;
}

int KeyboardInput::getKeyPress()
{
    if (!isAvailable())
    {
        return 0;
    }

    // Get the current key code from the keyboard controller
    int currentKeyCode = keyboard->getKey();

    // If no key is pressed, reset state
    if (currentKeyCode == 0)
    {
        lastKeyCode = 0;
        return 0;
    }

    // Check if this is a new key press (different from last key or enough time has passed)
    unsigned long currentTime = millis();
    if (currentKeyCode != lastKeyCode || (currentTime - lastKeyTime) > keyDebounceDelay)
    {
        lastKeyCode = currentKeyCode;
        lastKeyTime = currentTime;

        Serial.print("KeyboardInput: Key press detected - code: ");
        Serial.println(currentKeyCode);

        return currentKeyCode;
    }

    // Same key still pressed within debounce period
    return 0;
}

bool KeyboardInput::isAvailable()
{
    return keyboard != nullptr && keyboard->idVendor() != 0;
}
