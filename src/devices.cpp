#include "devices.h"
#include "input/gamepad_input.h"
#include "input/keyboard_input.h"

DeviceManager::DeviceManager()
    : host(nullptr), hub(nullptr), keyboard(nullptr),
      mouse(nullptr), joystick(nullptr)
{
}

DeviceManager::~DeviceManager()
{
}

void DeviceManager::setup()
{
    lcd->init();
    lcd->backlight();
    host->begin();

    gamepadInput = new GamepadInput(joystick);
    gamepadInput->setup();

    keyboardInput = new KeyboardInput(keyboard);

    Mouse.begin();
}

void DeviceManager::loop()
{
    host->Task();
}