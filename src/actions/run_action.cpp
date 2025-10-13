#include "actions/run_action.h"
#include "mapping_config.h"
#include "devices.h"
#include "joystick_mappings.h"

RunAction::RunAction(DeviceManager* dev, ActionHandler* hdlr, RunActionParams p)
  : Action(dev, hdlr), params(p), numMappings(0),
    mouseSensitivity(0.15),
    mouseDeadzone(16), 
    axisCenterValue(128),
    lastMouseUpdate(0),
    mouseUpdateInterval(1000 / 60) { 
}

void RunAction::init() {
    Serial.println("Run mode: Initialized");
        
    // Initialize Mouse
    Mouse.begin();
  
    // Try to load from SD card
    if (!MappingConfig::loadMappings("/config.json", mappings, numMappings, MAX_MAPPINGS)) {
        Serial.println("Failed to load config, using defaults");
        initializeDefaultMappings();
        
        // Optionally save the defaults
        MappingConfig::saveMappings("/config.json", mappings, numMappings);
    }
}

void RunAction::initializeDefaultMappings() {
    // Your default mappings as fallback
    numMappings = 0;
    
    mappings[numMappings++] = {Xbox360::Y, 'w', false};
    mappings[numMappings++] = {Xbox360::A, 's', false};
    mappings[numMappings++] = {Xbox360::X, 'a', false};
    mappings[numMappings++] = {Xbox360::B, 'd', false};
    
    mappings[numMappings++] = {Xbox360::DPAD_UP, KEY_UP, false};
    mappings[numMappings++] = {Xbox360::DPAD_DOWN, KEY_DOWN, false};
    mappings[numMappings++] = {Xbox360::DPAD_LEFT, KEY_LEFT, false};
    mappings[numMappings++] = {Xbox360::DPAD_RIGHT, KEY_RIGHT, false};
    
    mappings[numMappings++] = {Xbox360::LB, 'q', false};
    mappings[numMappings++] = {Xbox360::RB, 'e', false};
    
    mappings[numMappings++] = {Xbox360::START, KEY_RETURN, false};
    mappings[numMappings++] = {Xbox360::BACK, KEY_ESC, false};
    
    mappings[numMappings++] = {Xbox360::LEFT_STICK, KEY_LEFT_SHIFT, false};
    mappings[numMappings++] = {Xbox360::RIGHT_STICK, ' ', false};
    
    Serial.print("Initialized ");
    Serial.print(numMappings);
    Serial.println(" default mappings");
}

void RunAction::loop() {
    JoystickController* joy = devices->getJoystick();

    if (joy && *joy) {
        if (joy->available())
        {
            uint32_t buttons = joy->getButtons();
            // uint32_t static lastButtons = 0;
            // if (buttons != lastButtons)
            // {
            //     Serial.print("Buttons: ");
            //     Serial.println(buttons);
            //     lastButtons = buttons;
            // }

            // Check each mapped button
            for (int i = 0; i < numMappings; i++) {
                uint8_t buttonNum = mappings[i].joystickButton;
                bool isPressed = (buttons & (1 << buttonNum)) != 0;
                
                // Detect state change
                if (isPressed != mappings[i].currentlyPressed) {
                    mappings[i].currentlyPressed = isPressed;
                    processButtonMapping(buttonNum, isPressed);
                }
            }
        
            // Process mouse movement from left analog stick
            processMouseMovement();
        }
    }
}

int RunAction::applyDeadzone(int value, int deadzone) {
    // Center the value around 0
    int centered = value - axisCenterValue;
    
    // Apply deadzone
    if (abs(centered) < deadzone) {
        return 0;
    }
    
    // Return the adjusted value
    return centered;
}

void RunAction::processMouseMovement() {
    // Check if enough time has passed since last update
    unsigned long currentTime = millis();
    if (currentTime - lastMouseUpdate < mouseUpdateInterval) {
        return;  // Skip this update
    }
    
    lastMouseUpdate = currentTime;
    
    JoystickController* joy = devices->getJoystick();
    
    // int dPad = joy->getAxis(9);

    int rawX = joy->getAxis(Xbox360::AXIS_LEFT_X);
    int rawY = joy->getAxis(Xbox360::AXIS_LEFT_Y);
    
    int adjustedX = applyDeadzone(rawX, mouseDeadzone);
    int adjustedY = applyDeadzone(rawY, mouseDeadzone);
    
    if (adjustedX != 0 || adjustedY != 0) {
        int mouseX = (int)(adjustedX * mouseSensitivity);
        int mouseY = (int)(adjustedY * mouseSensitivity);
        
        Mouse.move(mouseX, mouseY, 0);

        // Serial.print("Stick: X=");
        // Serial.print(adjustedX);
        // Serial.print(" Y=");
        // Serial.print(adjustedY);
        // Serial.print(" | Mouse: X=");
        // Serial.print(mouseX);
        // Serial.print(" Y=");
        // Serial.println(mouseY);
    }
}
void RunAction::processButtonMapping(uint8_t buttonNum, bool isPressed) {
  // Find the mapping for this button
  for (int i = 0; i < numMappings; i++) {
    if (mappings[i].joystickButton == buttonNum) {
      if (isPressed) {
        // Serial.print("Button ");
        // Serial.print(buttonNum);
        // Serial.print(" pressed -> Key ");
        // Serial.println(mappings[i].keyCode);
        
        Keyboard.press(mappings[i].keyCode);
      } else {
        // Serial.print("Button ");
        // Serial.print(buttonNum);
        // Serial.println(" released");
        
        Keyboard.release(mappings[i].keyCode);
      }
      break;
    }
  }
}