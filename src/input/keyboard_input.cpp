#include "input/keyboard_input.h"

// Static member initialization
int KeyboardInput::pendingKeyCode = 0;
unsigned long KeyboardInput::pendingKeyTime = 0;

KeyboardInput::KeyboardInput(KeyboardController *kbd)
    : keyboard(kbd)
{
}

void KeyboardInput::setup()
{
    reset();

    // Attach callback handlers if keyboard is available
    if (keyboard != nullptr)
    {
        keyboard->attachPress(onKeyPress);
        keyboard->attachRelease(onKeyRelease);
        Serial.println("KeyboardInput: Callback handlers attached");
    } else {
        Serial.println("ERROR: KeyboardInput: not able to attach handlers");
    }
}

void KeyboardInput::reset()
{
    pendingKeyCode = 0;
    pendingKeyTime = 0;
}

void KeyboardInput::onKeyPress(int unicode)
{
    // Store the key press
    pendingKeyCode = unicode;
    pendingKeyTime = millis();

    Serial.print("KeyboardInput: Key press callback - code: ");
    Serial.println(unicode);
}

void KeyboardInput::onKeyRelease(int unicode)
{
    // Don't need to do anything on release for now
    Serial.print("KeyboardInput: Key release callback - code: ");
    Serial.println(unicode);
}

int KeyboardInput::getKeyPress()
{
    if (!isAvailable())
    {
        return 0;
    }

    // Check if there's a pending key press from the callback
    if (pendingKeyCode != 0)
    {
        int keyCode = pendingKeyCode;
        pendingKeyCode = 0; // Clear it so it's only returned once

        Serial.print("KeyboardInput: Returning key press: ");
        Serial.println(keyCode);

        return keyCode;
    }

    return 0;
}

bool KeyboardInput::isAvailable()
{
    return keyboard != nullptr && keyboard->idVendor() != 0;
}
