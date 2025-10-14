#include "devices.h"
#include "input.h"

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

    input = new Input(joystick);
    input->setup();
}

void DeviceManager::loop()
{
    host->Task();
}