#include "actions/bind_key_action.h"
#include "actions/action_handler.h"
#include "mapping/joystick_mappings.h"
#include "mapping/keyboard_mapping.h"
#include "devices.h"

BindKeyAction::BindKeyAction(DeviceManager *dev, ActionHandler *hdlr, BindKeyActionParams p)
    : Action(dev, hdlr), params(p), lastDisplayUpdate(0)
{
}

BindKeyAction::~BindKeyAction()
{
}

void BindKeyAction::init()
{
    // Validate parameters based on target type
    if (params.target == BindKeyTarget::BUTTON_MAPPING)
    {
        if (params.mappingIndex < 0 || params.mappingIndex >= mappingConfig.numMappings)
        {
            handler->popAction();
            return;
        }
    }

    KeyboardInput* keyboardInput = devices->getKeyboardInput();
    if (keyboardInput != nullptr)
    {
        keyboardInput->reset();
    }

    updateDisplay();
}

void BindKeyAction::loop()
{
    KeyboardInput* keyboardInput = devices->getKeyboardInput();

    if (keyboardInput != nullptr)
    {
        int unicode = keyboardInput->getKeyPress();
        if (unicode != 0)
        {
            int keyCode = KeyboardMapping::unicodeToKeyCode(unicode);

            Serial.print("BindKeyAction: Received unicode ");
            Serial.print(unicode);
            Serial.print(", 0x");
            Serial.print(unicode, HEX);
            Serial.print(", converted to keyCode ");
            Serial.println(keyCode);

            applyKeyBinding(keyCode);

            handler->popAction();
        }
    }

    // Allow cancel with gamepad button
    GamepadInputEvent event = devices->getGamepadInput()->getEvent();
    if (event == INPUT_CANCEL)
    {
        handler->popAction();
    }
}

void BindKeyAction::setParams(BindKeyActionParams p)
{
    params = p;
    Serial.print("BindKeyAction: setParams() called with mappingIndex: ");
    Serial.println(params.mappingIndex);
}

void BindKeyAction::updateDisplay()
{
    LiquidCrystal_I2C *lcd = devices->getLCD();
    if (lcd == nullptr)
    {
        return;
    }

    lcd->clear();

    // Get the target name based on what we're binding
    String targetName;
    if (params.target == BindKeyTarget::BUTTON_MAPPING)
    {
        const char *buttonName = JoystickMapping::getGenericButtonName(mappingConfig.mappings[params.mappingIndex].genericButton);
        targetName = String(buttonName);
    }
    else if (params.target == BindKeyTarget::TRIGGER_LEFT)
    {
        targetName = "Left Trigger";
    }
    else if (params.target == BindKeyTarget::TRIGGER_RIGHT)
    {
        targetName = "Right Trigger";
    }

    // Line 1: "Bind key for:"
    lcd->setCursor(0, 0);
    lcd->print("Bind key for:");

    // Line 2: Target name
    lcd->setCursor(0, 1);
    lcd->print(targetName);

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
    Serial.print(") to ");

    String targetName;

    // Update the mapping configuration based on target type
    if (params.target == BindKeyTarget::BUTTON_MAPPING)
    {
        const char *buttonName = JoystickMapping::getGenericButtonName(mappingConfig.mappings[params.mappingIndex].genericButton);
        Serial.println(buttonName);
        targetName = String(buttonName);
        mappingConfig.mappings[params.mappingIndex].keyCode = keyCode;
    }
    else if (params.target == BindKeyTarget::TRIGGER_LEFT)
    {
        Serial.println("Left Trigger");
        targetName = "Left Trigger";
        mappingConfig.triggers.keyLeft = keyCode;
    }
    else if (params.target == BindKeyTarget::TRIGGER_RIGHT)
    {
        Serial.println("Right Trigger");
        targetName = "Right Trigger";
        mappingConfig.triggers.keyRight = keyCode;
    }

    // Mark config as modified
    mappingConfig.modified = true;
    Serial.println("BindKeyAction: Config marked as modified");

    // Show confirmation on LCD
    LiquidCrystal_I2C *lcd = devices->getLCD();
    if (lcd != nullptr)
    {
        lcd->clear();
        lcd->setCursor(0, 0);
        lcd->print("Key bound!");
        lcd->setCursor(0, 1);
        lcd->print(targetName);
        lcd->print(" > ");
        lcd->print(KeyboardMapping::keyCodeToString(keyCode));

        delay(1000);
    }
}
