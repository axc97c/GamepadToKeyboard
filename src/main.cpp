#include <USBHost_t36.h>
#include <../lib/LCD_I2C-master/LiquidCrystal_I2C.h>

#include "main.h"

#include "devices.h"
#include "actions/action_handler.h"
#include "actions/run_action.h"
#include "mapping/mapping_config.h"
#include "memory.h"

USBHost usbh;
USBHub hub1(usbh);
USBHub hub2(usbh);
USBHIDParser hid1(usbh);
USBHIDParser hid2(usbh);
USBHIDParser hid3(usbh);
KeyboardController keyboard(usbh);
MouseController mouse(usbh);
JoystickController joy(usbh);
LiquidCrystal_I2C lcd(0x27, 20, 4);

DeviceManager devices;
ActionHandler actionHandler(&devices);
JoystickMappingConfig mappingConfig;

void setup()
{
    devices.host = &usbh;
    devices.keyboard = &keyboard;
    devices.mouse = &mouse;
    devices.joystick = &joy;
    devices.lcd = &lcd;

    Serial.begin(115200);
    while (!Serial && millis() < 3000)
    {
    }; // Probably dont want this long term

    Serial.println("Main: === GamePad to Keyboard Starting ===");
    Serial.println("Main: Initializing USB Host...");

    MappingConfig::initSD();

    devices.setup();
    actionHandler.setup();

    // Initialize memory monitoring (reports every 10 seconds by default)
    MemoryMonitor::init();

    Serial.println("Main: Setup complete. Waiting for USB devices...");
}

void loop()
{
    devices.loop();
    actionHandler.loop();

    // Update memory monitor (prints report every 10 seconds)
    MemoryMonitor::update();
}