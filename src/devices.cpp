#include "devices.h"
#include "input/gamepad_input.h"
#include "input/keyboard_input.h"

DeviceManager::DeviceManager()
    : host(nullptr), keyboard(nullptr),
      mouse(nullptr), joystick(nullptr),
      keyboardConnected(false), mouseConnected(false), joystickConnected(false)
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
    keyboardInput = new KeyboardInput(keyboard);
    keyboardInput->setup();

    Mouse.begin();
}

void DeviceManager::loop()
{
    host->Task();
    checkDeviceConnections();
}

void DeviceManager::checkDeviceConnections()
{
    // Check keyboard connection status
    // Use idVendor() to detect if a device is connected (returns 0 if not connected)
    bool keyboardNowConnected = (keyboard != nullptr && keyboard->idVendor() != 0);
    if (keyboardNowConnected && !keyboardConnected)
    {
        Serial.println("DeviceManager: [USB] Keyboard connected");
        Serial.print("DeviceManager: [USB] VID: 0x");
        Serial.print(keyboard->idVendor(), HEX);
        Serial.print(", PID: 0x");
        Serial.println(keyboard->idProduct(), HEX);
        Serial.print("DeviceManager: [USB] Timestamp: ");
        Serial.println(millis());
        keyboardConnected = true;
    }
    else if (!keyboardNowConnected && keyboardConnected)
    {
        Serial.println("DeviceManager: [USB] Keyboard disconnected");
        Serial.print("DeviceManager: [USB] Timestamp: ");
        Serial.println(millis());
        keyboardConnected = false;
    }

    // MouseController uses available() method instead of idVendor()
    bool mouseNowConnected = (mouse != nullptr && mouse->available());
    if (mouseNowConnected && !mouseConnected)
    {
        Serial.println("DeviceManager: [USB] Mouse connected");
        Serial.print("DeviceManager: [USB] Timestamp: ");
        Serial.println(millis());
        mouseConnected = true;
    }
    else if (!mouseNowConnected && mouseConnected)
    {
        Serial.println("DeviceManager: [USB] Mouse disconnected");
        Serial.print("DeviceManager: [USB] Timestamp: ");
        Serial.println(millis());
        mouseConnected = false;
    }
    
    // Check joystick connection status
    bool joystickNowConnected = (joystick != nullptr && joystick->idVendor() != 0);
    if (joystickNowConnected && !joystickConnected)
    {
        Serial.println("DeviceManager: [USB] Joystick/Gamepad connected");
        Serial.print("DeviceManager: [USB] VID: 0x");
        Serial.print(joystick->idVendor(), HEX);
        Serial.print(", PID: 0x");
        Serial.println(joystick->idProduct(), HEX);
        Serial.print("DeviceManager: [USB] Timestamp: ");
        Serial.println(millis());
        joystickConnected = true;
    }
    else if (!joystickNowConnected && joystickConnected)
    {
        Serial.println("DeviceManager: [USB] Joystick/Gamepad disconnected");
        Serial.print("DeviceManager: [USB] Timestamp: ");
        Serial.println(millis());
        joystickConnected = false;
    }
}