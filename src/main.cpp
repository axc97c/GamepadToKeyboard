#include <USBHost_t36.h>

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

DeviceManager devices;

ActionHandler actionHandler(&devices);

void setup() {
    devices.host = &usbh;
    devices.hub = &hub;
    devices.keyboard = &keyboard;
    devices.mouse = &mouse;
    devices.joystick = &joy;

    Serial.begin(115200);
    while (!Serial && millis() < 30000) {}; // Probably dont want this long term

    MappingConfig::initSD();

    devices.setup();
    actionHandler.setup();

    Serial.println("Attaching keyboard callbacks...");
    devices.getKeyboard()->attachPress(onPress);
    devices.getKeyboard()->attachRelease(onRelease);

    Serial.println("=== Setup Complete ===");
}

void loop() {
    devices.loop();
    actionHandler.loop();

    KeyboardController* kbd = devices.getKeyboard();
    if (kbd && *kbd) {
        static bool printed = false;
        if (!printed) {
            printKeyboardInfo();
            printed = true;
        }
    }
}

void onPress(int key) {
    Serial.print("Key pressed: ");
    Serial.println(key);
    
    // Get modifiers
    uint8_t mods = keyboard.getModifiers();
    
    // Press modifiers first
    if (mods & 0x01) Keyboard.press(KEY_LEFT_CTRL);
    if (mods & 0x02) Keyboard.press(KEY_LEFT_SHIFT);
    if (mods & 0x04) Keyboard.press(KEY_LEFT_ALT);
    if (mods & 0x08) Keyboard.press(KEY_LEFT_GUI);
    if (mods & 0x10) Keyboard.press(KEY_RIGHT_CTRL);
    if (mods & 0x20) Keyboard.press(KEY_RIGHT_SHIFT);
    if (mods & 0x40) Keyboard.press(KEY_RIGHT_ALT);
    if (mods & 0x80) Keyboard.press(KEY_RIGHT_GUI);

    Keyboard.press(key);
}

void onRelease(int key) {
    Serial.print("Key released: ");
    Serial.println(key);
    
    Keyboard.release(key);
    
    // Get current modifiers and release any that are no longer pressed
    uint8_t mods = keyboard.getModifiers();
    
    if (!(mods & 0x01)) Keyboard.release(KEY_LEFT_CTRL);
    if (!(mods & 0x02)) Keyboard.release(KEY_LEFT_SHIFT);
    if (!(mods & 0x04)) Keyboard.release(KEY_LEFT_ALT);
    if (!(mods & 0x08)) Keyboard.release(KEY_LEFT_GUI);
    if (!(mods & 0x10)) Keyboard.release(KEY_RIGHT_CTRL);
    if (!(mods & 0x20)) Keyboard.release(KEY_RIGHT_SHIFT);
    if (!(mods & 0x40)) Keyboard.release(KEY_RIGHT_ALT);
    if (!(mods & 0x80)) Keyboard.release(KEY_RIGHT_GUI);
}

void printKeyboardInfo() {

    KeyboardController* keyboard = devices.getKeyboard();

    Serial.println("\n=== Keyboard Connected ===");
    
    // Vendor ID and Product ID
    uint16_t vid = keyboard->idVendor();
    uint16_t pid = keyboard->idProduct();
    
    Serial.print("Vendor ID: 0x");
    Serial.println(vid, HEX);
    Serial.print("Product ID: 0x");
    Serial.println(pid, HEX);
    
    // Manufacturer and Product strings (if available)
    const uint8_t *manufacturer = keyboard->manufacturer();
    const uint8_t *product = keyboard->product();
    
    if (manufacturer) {
        Serial.print("Manufacturer: ");
        Serial.println((const char *)manufacturer);
    }
    
    if (product) {
        Serial.print("Product: ");
        Serial.println((const char *)product);
    }
    
    Serial.println("========================\n");
}