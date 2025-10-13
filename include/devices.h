#ifndef DEVICES_H
#define DEVICES_H

#include <USBHost_t36.h>

class DeviceManager {
  public:
    USBHost* host;
    USBHub* hub;
    USBHIDParser* parser;
    KeyboardController* keyboard;
    MouseController* mouse;
    JoystickController* joystick;

    DeviceManager();
    ~DeviceManager();  // Destructor to clean up
    
    void setup();
    void loop();
    
    // Getters to access the controllers
    KeyboardController* getKeyboard() { return keyboard; }
    MouseController* getMouse() { return mouse; }
    JoystickController* getJoystick() { return joystick; }
};

#endif