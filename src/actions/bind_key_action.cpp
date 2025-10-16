#include "actions/bind_key_action.h"
#include "actions/action_handler.h"
#include "mapping/joystick_mappings.h"
#include "mapping/keyboard_mapping.h"
#include "devices.h"

BindKeyAction::BindKeyAction(DeviceManager *dev, ActionHandler *hdlr, BindKeyActionParams p)
    : Action(dev, hdlr), params(p), displayInitialized(false), lastDisplayUpdate(0)
{
}

BindKeyAction::~BindKeyAction()
{
}

void BindKeyAction::init()
{
    Serial.println("BindKeyAction: Initializing key binding mode...");

    // Validate mapping index
    if (params.mappingIndex < 0 || params.mappingIndex >= mappingConfig.numMappings)
    {
        Serial.println("BindKeyAction: Invalid mapping index!");
        handler->popAction();
        return;
    }

    // Reset keyboard input state to clear any pending keys from callbacks
    KeyboardInput* keyboardInput = devices->getKeyboardInput();
    if (keyboardInput != nullptr)
    {
        keyboardInput->reset();
        Serial.println("BindKeyAction: Keyboard input state reset");
    }

    // Display initial prompt
    updateDisplay();
    displayInitialized = true;

    Serial.print("BindKeyAction: Waiting for key press for button: ");
    Serial.println(JoystickMapping::getGenericButtonName(mappingConfig.mappings[params.mappingIndex].genericButton));
}

void BindKeyAction::loop()
{
    KeyboardInput* keyboardInput = devices->getKeyboardInput();

    // Check for keyboard input
    if (keyboardInput != nullptr)
    {
        int unicode = keyboardInput->getKeyPress();
        if (unicode != 0)
        {
            // Convert unicode/ASCII to Teensy keycode
            int keyCode = KeyboardMapping::unicodeToKeyCode(unicode);

            Serial.print("BindKeyAction: Received unicode ");
            Serial.print(unicode);
            Serial.print(", 0x");
            Serial.print(unicode, HEX);
            Serial.print(", converted to keyCode ");
            Serial.println(keyCode);

            // Key was pressed, apply the binding
            applyKeyBinding(keyCode);

            // Return to previous action (edit config menu)
            handler->popAction();
        }
    }

    // Allow cancel with gamepad button
    GamepadInputEvent event = devices->getGamepadInput()->getEvent();
    if (event == INPUT_CANCEL)
    {
        Serial.println("BindKeyAction: Cancelled by user");
        handler->popAction();
    }
}

ActionType BindKeyAction::getType()
{
    return ActionType::BIND_KEY;
}

void BindKeyAction::updateDisplay()
{
    LiquidCrystal_I2C *lcd = devices->getLCD();
    if (lcd == nullptr)
    {
        return;
    }

    lcd->clear();

    // Get the button name
    const char *buttonName = JoystickMapping::getGenericButtonName(mappingConfig.mappings[params.mappingIndex].genericButton);
    String shortButtonName = String(buttonName);

    // Remove "BTN_" prefix if present
    if (shortButtonName.startsWith("BTN_"))
    {
        shortButtonName = shortButtonName.substring(4);
    }

    // Line 1: "Bind key for:"
    lcd->setCursor(0, 0);
    lcd->print("Bind key for:");

    // Line 2: Button name (centered if short enough)
    lcd->setCursor(0, 1);
    lcd->print(shortButtonName);

    // Line 3: "Press a key..."
    lcd->setCursor(0, 2);
    lcd->print("Press a key...");

    // Line 4: "or B to cancel"
    lcd->setCursor(0, 3);
    lcd->print("or B to cancel");
}

void BindKeyAction::applyKeyBinding(int keyCode)
{
    Serial.print("BindKeyAction: Binding key ");
    Serial.print(KeyboardMapping::keyCodeToString(keyCode));
    Serial.print(" (code: ");
    Serial.print(keyCode);
    Serial.print(") to button ");
    Serial.println(JoystickMapping::getGenericButtonName(mappingConfig.mappings[params.mappingIndex].genericButton));

    // Update the mapping configuration
    mappingConfig.mappings[params.mappingIndex].keyCode = keyCode;

    // Mark config as modified
    mappingConfig.modified = true;
    Serial.println("Config marked as modified");

    // Show confirmation on LCD
    LiquidCrystal_I2C *lcd = devices->getLCD();
    if (lcd != nullptr)
    {
        lcd->clear();
        lcd->setCursor(0, 0);
        lcd->print("Key bound!");
        lcd->setCursor(0, 1);

        const char *buttonName = JoystickMapping::getGenericButtonName(mappingConfig.mappings[params.mappingIndex].genericButton);
        String shortButtonName = String(buttonName);
        if (shortButtonName.startsWith("BTN_"))
        {
            shortButtonName = shortButtonName.substring(4);
        }

        lcd->print(shortButtonName);
        lcd->print(" > ");
        lcd->print(KeyboardMapping::keyCodeToString(keyCode));

        delay(1000); // Show confirmation for 1 second
    }

    // Note: We don't save to file here - that would be done separately
    // The mappingConfig is updated in memory and will be used immediately
}
