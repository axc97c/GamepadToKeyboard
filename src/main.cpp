#include <USBHost_t36.h>
#include <../lib/LCD_I2C-master/LiquidCrystal_I2C.h>

#include "main.h"

#include "devices.h"
#include "actions/action_handler.h"
#include "actions/run_action.h"
#include "mapping/mapping_config.h"

USBHost usbh;
USBHub hub(usbh);
USBHIDParser hid(usbh);
KeyboardController keyboard(usbh);
MouseController mouse(usbh);
JoystickController joy(usbh);
LiquidCrystal_I2C lcd(0x27, 20, 4);

DeviceManager devices;
ActionHandler actionHandler(&devices);

// Global joystick mapping configuration
JoystickMappingConfig mappingConfig;

void setup()
{
    devices.host = &usbh;
    devices.hub = &hub;
    devices.keyboard = &keyboard;
    devices.mouse = &mouse;
    devices.joystick = &joy;
    devices.lcd = &lcd;

    Serial.begin(115200);
    while (!Serial && millis() < 3000)
    {
    }; // Probably dont want this long term

    MappingConfig::initSD();

    devices.setup();
    actionHandler.setup();
}

void loop()
{
    devices.loop();
    actionHandler.loop();
}