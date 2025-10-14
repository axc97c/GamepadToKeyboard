#ifndef DEVICES_H
#define DEVICES_H

#include <USBHost_t36.h>
#include <../lib/LCD_I2C-master/LiquidCrystal_I2C.h>
#include "input.h"

class DeviceManager
{
public:
    // USB host devices
    USBHost *host;
    USBHub *hub;
    USBHIDParser *parser;
    KeyboardController *keyboard;
    MouseController *mouse;
    JoystickController *joystick;

    // Non USB devices to share around
    LiquidCrystal_I2C *lcd;
    Input *input;

    DeviceManager();
    ~DeviceManager(); // Destructor to clean up

    void setup();
    void loop();

    // Getters to access the controllers
    KeyboardController *getKeyboard() { return keyboard; }
    MouseController *getMouse() { return mouse; }
    JoystickController *getJoystick() { return joystick; }
    LiquidCrystal_I2C *getLCD() { return lcd; }
    Input *getInput() { return input; }
};

#endif