#include "input/keyboard_input.h"

int KeyboardInput::pendingKeyCode = 0;
unsigned long KeyboardInput::pendingKeyTime = 0;

KeyboardInput::KeyboardInput(KeyboardController *kbd)
    : keyboard(kbd)
{
    reset();
}

void KeyboardInput::setup()
{
    if (keyboard != nullptr)
    {
        keyboard->attachPress(onKeyPress);
        keyboard->attachRelease(onKeyRelease);
    }
}

void KeyboardInput::reset()
{
    pendingKeyCode = 0;
    pendingKeyTime = 0;
}

void KeyboardInput::onKeyPress(int unicode)
{
    pendingKeyCode = unicode;
    pendingKeyTime = millis();
}

void KeyboardInput::onKeyRelease(int unicode)
{
}

int KeyboardInput::getKeyPress()
{
    if (!isAvailable())
    {
        return 0;
    }

    if (pendingKeyCode != 0)
    {
        int keyCode = pendingKeyCode;
        pendingKeyCode = 0;

        return keyCode;
    }

    return 0;
}

bool KeyboardInput::isAvailable()
{
    return keyboard != nullptr && keyboard->idVendor() != 0;
}
