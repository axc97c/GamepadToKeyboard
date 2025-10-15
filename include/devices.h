#ifndef DEVICES_H
#define DEVICES_H

#include <USBHost_t36.h>
#include <../lib/LCD_I2C-master/LiquidCrystal_I2C.h>
#include "input/gamepad_input.h"
#include "input/keyboard_input.h"

class DeviceManager
{
public:
    // USB host devices
    USBHost *host;
    KeyboardController *keyboard;
    MouseController *mouse;
    JoystickController *joystick;

    // Non USB devices to share around
    LiquidCrystal_I2C *lcd;
    GamepadInput *gamepadInput;
    KeyboardInput *keyboardInput;

    DeviceManager();
    ~DeviceManager(); // Destructor to clean up

    void setup();
    void loop();

private:
    // Track device connection states
    bool keyboardConnected;
    bool mouseConnected;
    bool joystickConnected;

    // Check for device connection changes
    void checkDeviceConnections();

public:
    // Getters to access the controllers
    KeyboardController *getKeyboard() { return keyboard; }
    MouseController *getMouse() { return mouse; }
    JoystickController *getJoystick() { return joystick; }
    LiquidCrystal_I2C *getLCD() { return lcd; }
    GamepadInput *getGamepadInput() { return gamepadInput; }
    KeyboardInput *getKeyboardInput() { return keyboardInput; }
};

#endif