#include "actions/run_action.h"
#include "mapping_config.h"
#include "devices.h"
#include "joystick_mappings.h"

void RunAction::init() {
  Serial.println("Run mode: Initialized");
  
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

void RunAction::initializeMappings() {
    // Clear all mappings
    numMappings = 0;
        
    // Example mappings using named constants
    // Face buttons - WASD layout
    mappings[numMappings++] = {Xbox360::Y, 'w', false};        // Y -> W (forward)
    mappings[numMappings++] = {Xbox360::A, 's', false};        // A -> S (back)
    mappings[numMappings++] = {Xbox360::X, 'a', false};        // X -> A (left)
    mappings[numMappings++] = {Xbox360::B, 'd', false};        // B -> D (right)
    
    // D-Pad - Arrow keys
    mappings[numMappings++] = {Xbox360::DPAD_UP, KEY_UP, false};
    mappings[numMappings++] = {Xbox360::DPAD_DOWN, KEY_DOWN, false};
    mappings[numMappings++] = {Xbox360::DPAD_LEFT, KEY_LEFT, false};
    mappings[numMappings++] = {Xbox360::DPAD_RIGHT, KEY_RIGHT, false};
    
    // Shoulder buttons
    mappings[numMappings++] = {Xbox360::LB, 'q', false};       // LB -> Q
    mappings[numMappings++] = {Xbox360::RB, 'e', false};       // RB -> E
    
    // Start/Back
    mappings[numMappings++] = {Xbox360::START, KEY_RETURN, false};    // Start -> Enter
    mappings[numMappings++] = {Xbox360::BACK, KEY_ESC, false};        // Back -> Escape
    
    // Stick clicks
    mappings[numMappings++] = {Xbox360::LEFT_STICK, KEY_LEFT_SHIFT, false};
    mappings[numMappings++] = {Xbox360::RIGHT_STICK, KEY_SPACE, false};
    
    // Xbox button
    mappings[numMappings++] = {Xbox360::XBOX_BUTTON, KEY_TAB, false};

    Serial.print("Initialized ");
    Serial.print(numMappings);
    Serial.println(" button mappings");
}

void RunAction::loop() {
  JoystickController* joy = devices->getJoystick();

  if (joy && *joy) {
    // Get button state - this depends on the joystick library
    // The USBHost_t36 library provides these methods:
    uint32_t buttons = joy->getButtons();
    
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
  }
}

void RunAction::processButtonMapping(uint8_t buttonNum, bool isPressed) {
  // Find the mapping for this button
  for (int i = 0; i < numMappings; i++) {
    if (mappings[i].joystickButton == buttonNum) {
      if (isPressed) {
        Serial.print("Button ");
        Serial.print(buttonNum);
        Serial.print(" pressed -> Key ");
        Serial.println(mappings[i].keyCode);
        
        Keyboard.press(mappings[i].keyCode);
      } else {
        Serial.print("Button ");
        Serial.print(buttonNum);
        Serial.println(" released");
        
        Keyboard.release(mappings[i].keyCode);
      }
      break;
    }
  }
}