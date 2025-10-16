#include "actions/text_input_action.h"
#include "actions/action_handler.h"
#include "mapping/keyboard_mapping.h"
#include "devices.h"

TextInputAction::TextInputAction(DeviceManager *dev, ActionHandler *hdlr, TextInputActionParams p)
    : Action(dev, hdlr), params(p), cursorPosition(0), displayInitialized(false),
      lastBlinkTime(0), cursorVisible(true)
{
}

TextInputAction::~TextInputAction()
{
}

void TextInputAction::init()
{
    Serial.println("TextInputAction: Initializing text input mode...");

    inputText = "";
    cursorPosition = 0;

    // Reset keyboard input state
    KeyboardInput* keyboardInput = devices->getKeyboardInput();
    if (keyboardInput != nullptr)
    {
        keyboardInput->reset();
        Serial.println("TextInputAction: Keyboard input state reset");
    }

    // Display initial prompt
    updateDisplay();
    displayInitialized = true;

    Serial.print("TextInputAction: Waiting for text input. Max length: ");
    Serial.println(params.maxLength);
}

void TextInputAction::loop()
{
    KeyboardInput* keyboardInput = devices->getKeyboardInput();

    // Check for keyboard input
    if (keyboardInput != nullptr)
    {
        int unicode = keyboardInput->getKeyPress();
        if (unicode != 0)
        {
            Serial.print("TextInputAction: Received unicode ");
            Serial.print(unicode);
            Serial.print(" ('");
            if (unicode >= 32 && unicode < 127)
            {
                Serial.print((char)unicode);
            }
            Serial.println("')");

            handleKeyPress(unicode);
        }
    }

    // Allow cancel with gamepad button
    GamepadInputEvent event = devices->getGamepadInput()->getEvent();
    if (event == INPUT_CANCEL)
    {
        Serial.println("TextInputAction: Cancelled by user");
        cancelInput();
    }

    // Update blinking cursor
    unsigned long currentTime = millis();
    if (currentTime - lastBlinkTime >= blinkInterval)
    {
        cursorVisible = !cursorVisible;
        lastBlinkTime = currentTime;
        updateDisplay();
    }
}

ActionType TextInputAction::getType()
{
    return ActionType::BIND_KEY; // Reuse BIND_KEY type since it's similar input mode
}

void TextInputAction::handleKeyPress(int unicode)
{
    Serial.print("TextInputAction: Handling unicode: ");
    Serial.println(unicode);

    // Handle special keys (by unicode value)
    if (unicode == 13 || unicode == 10) // Enter/Return
    {
        finishInput();
        return;
    }
    else if (unicode == 8 || unicode == 127) // Backspace/Delete
    {
        // Remove last character
        if (inputText.length() > 0)
        {
            inputText.remove(inputText.length() - 1);
            cursorPosition = inputText.length();
            updateDisplay();
        }
        return;
    }
    else if (unicode == 27) // ESC
    {
        cancelInput();
        return;
    }

    // Check if we can add more characters
    if (inputText.length() >= params.maxLength)
    {
        Serial.println("TextInputAction: Max length reached");
        return;
    }

    // Convert unicode to character - handle allowed characters
    char c = 0;

    // Uppercase letters (A-Z) - ASCII 65-90
    if (unicode >= 'A' && unicode <= 'Z')
    {
        c = (char)unicode;
    }
    // Lowercase letters (a-z) - ASCII 97-122
    else if (unicode >= 'a' && unicode <= 'z')
    {
        c = (char)unicode;
    }
    // Numbers (0-9) - ASCII 48-57
    else if (unicode >= '0' && unicode <= '9')
    {
        c = (char)unicode;
    }
    // Space - convert to underscore for filename safety
    else if (unicode == ' ' || unicode == 32)
    {
        c = '_';
    }
    // Underscore
    else if (unicode == '_' || unicode == 95)
    {
        c = '_';
    }
    // Dash/Hyphen
    else if (unicode == '-' || unicode == 45)
    {
        c = '-';
    }
    // Period/Dot
    else if (unicode == '.' || unicode == 46)
    {
        c = '.';
    }

    // If we got a valid character, add it
    if (c != 0)
    {
        inputText += c;
        cursorPosition = inputText.length();
        updateDisplay();

        Serial.print("TextInputAction: Added '");
        Serial.print(c);
        Serial.print("', current input: ");
        Serial.println(inputText);
    }
    else
    {
        Serial.print("TextInputAction: Invalid/unsupported character (unicode ");
        Serial.print(unicode);
        Serial.println(")");
    }
}

void TextInputAction::updateDisplay()
{
    LiquidCrystal_I2C *lcd = devices->getLCD();
    if (lcd == nullptr)
    {
        return;
    }

    lcd->clear();

    // Line 0: Prompt
    lcd->setCursor(0, 0);
    lcd->print(params.prompt);

    // Line 1: Current input with blinking cursor
    lcd->setCursor(0, 1);
    lcd->print(inputText);
    if (cursorVisible && inputText.length() < params.maxLength)
    {
        lcd->print("_");
    }

    // Line 2: Character count
    lcd->setCursor(0, 2);
    lcd->print(inputText.length());
    lcd->print("/");
    lcd->print(params.maxLength);
    lcd->print(" chars");

    // Line 3: Instructions
    lcd->setCursor(0, 3);
    if (inputText.length() > 0)
    {
        lcd->print("Enter=OK B=Cancel");
    }
    else
    {
        lcd->print("Type or B=Cancel");
    }
}

void TextInputAction::finishInput()
{
    if (inputText.length() == 0)
    {
        Serial.println("TextInputAction: Empty input, cancelling");
        cancelInput();
        return;
    }

    Serial.print("TextInputAction: Input completed: ");
    Serial.println(inputText);

    // Copy result to buffer
    inputText.toCharArray(params.resultBuffer, params.maxLength + 1);

    // Pop this action to return to caller
    handler->popAction();
}

void TextInputAction::cancelInput()
{
    Serial.println("TextInputAction: Input cancelled");

    // Set empty result
    params.resultBuffer[0] = '\0';

    // Pop this action
    handler->popAction();
}
