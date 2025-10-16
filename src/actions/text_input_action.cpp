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
        int keyCode = keyboardInput->getKeyPress();
        if (keyCode != 0)
        {
            handleKeyPress(keyCode);
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

void TextInputAction::handleKeyPress(int keyCode)
{
    Serial.print("TextInputAction: Key pressed: ");
    Serial.print(KeyboardMapping::keyCodeToString(keyCode));
    Serial.print(" (code: ");
    Serial.print(keyCode);
    Serial.println(")");

    // Handle special keys
    if (keyCode == KEY_ENTER || keyCode == KEY_RETURN)
    {
        // Finish input
        finishInput();
        return;
    }
    else if (keyCode == KEY_BACKSPACE || keyCode == KEY_DELETE)
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
    else if (keyCode == KEY_ESC)
    {
        // Cancel input
        cancelInput();
        return;
    }

    // Check if we can add more characters
    if (inputText.length() >= params.maxLength)
    {
        Serial.println("TextInputAction: Max length reached");
        return;
    }

    // Convert key code to character
    // For simplicity, we'll handle alphanumeric keys
    char c = 0;

    // Letters (a-z)
    if (keyCode >= KEY_A && keyCode <= KEY_Z)
    {
        c = 'a' + (keyCode - KEY_A);
    }
    // Numbers (0-9) - top row keys
    else if (keyCode >= KEY_0 && keyCode <= KEY_9)
    {
        c = '0' + (keyCode - KEY_0);
    }
    // Underscore and dash
    else if (keyCode == KEY_MINUS)
    {
        c = '-';
    }
    else if (keyCode == KEY_SPACE)
    {
        c = '_'; // Replace space with underscore for filenames
    }

    // If we got a valid character, add it
    if (c != 0)
    {
        inputText += c;
        cursorPosition = inputText.length();
        updateDisplay();

        Serial.print("TextInputAction: Current input: ");
        Serial.println(inputText);
    }
    else
    {
        Serial.println("TextInputAction: Invalid character for filename");
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
