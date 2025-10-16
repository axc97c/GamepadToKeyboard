#include "input/gamepad_input.h"

GamepadInput::GamepadInput(JoystickController *controller)
{
    joystick = controller;
    lastButtons = 0;
    lastDPadValue = 0xFF;

    // Initialize button state arrays
    for (int i = 0; i < 32; i++)
    {
        buttonPressTime[i] = 0;
        buttonRepeating[i] = false;
    }
}

void GamepadInput::setup()
{
    // Don't check controller type here - joystick may not be connected yet
    // We'll check dynamically in isDPadPressed()

    Serial.println("GamepadInput: Input setup complete");
}

void GamepadInput::update()
{
    // This method can be used for any per-frame updates if needed
    // Currently, all logic is in getEvent()
}

void GamepadInput::reset()
{
    lastButtons = 0;
    lastDPadValue = 0xFF;

    for (int i = 0; i < 32; i++)
    {
        buttonPressTime[i] = 0;
        buttonRepeating[i] = false;
    }

    Serial.println("GamepadInput: Input state reset");
}

bool GamepadInput::isButtonPressed(uint8_t genericButton)
{
    if (!joystick->available())
        return false;

    uint32_t buttons = joystick->getButtons();
    JoystickController::joytype_t type = joystick->joystickType();

    // Use reverse mapping to get physical button number
    int physicalButton = JoystickMapping::mapGenericToButton(type, genericButton);

    if (physicalButton < 0)
    {
        return false; // Button not supported on this controller
    }

    // Check if the physical button is pressed
    return buttons & (1 << physicalButton);
}

bool GamepadInput::isDPadPressed(uint8_t dpadButton)
{
    if (!joystick->available())
        return false;

    JoystickController::joytype_t type = joystick->joystickType();
    uint8_t axisNumber;

    // Check if this controller type uses D-pad as axis
    if (JoystickMapping::usesDPadAxis(type, axisNumber))
    {
        // Read D-pad from axis
        uint64_t axisMask = joystick->axisMask();
        if (axisMask & (1 << axisNumber))
        {
            int axisValue = joystick->getAxis(axisNumber);
            int mappedButton = JoystickMapping::mapDPadValueToButton(type, axisValue);
            return mappedButton == dpadButton;
        }
    }
    else
    {
        // D-pad is buttons, use regular button check
        return isButtonPressed(dpadButton);
    }

    return false;
}

GamepadInputEvent GamepadInput::checkButton(uint8_t genericButton, GamepadInputEvent event)
{
    bool pressed = false;

    // Check if this is a D-pad button
    if (genericButton >= GenericController::BTN_DPAD_UP &&
        genericButton <= GenericController::BTN_DPAD_RIGHT)
    {
        pressed = isDPadPressed(genericButton);
    }
    else
    {
        pressed = isButtonPressed(genericButton);
    }

    if (!pressed)
    {
        // Button released
        buttonPressTime[genericButton] = 0;
        buttonRepeating[genericButton] = false;
        return INPUT_NONE;
    }

    unsigned long currentTime = millis();

    // First press
    if (buttonPressTime[genericButton] == 0)
    {
        buttonPressTime[genericButton] = currentTime;
        return event; // Return event immediately on first press
    }

    unsigned long heldTime = currentTime - buttonPressTime[genericButton];

    // Check if we should start repeating
    if (!buttonRepeating[genericButton] && heldTime >= repeatDelay)
    {
        buttonRepeating[genericButton] = true;
        return event; // Return event when repeat starts
    }

    // Handle repeating
    if (buttonRepeating[genericButton])
    {
        unsigned long timeSinceRepeatStart = heldTime - repeatDelay;
        if (timeSinceRepeatStart % repeatInterval < 20)
        { // Small window to catch repeat
            return event;
        }
    }

    return INPUT_NONE;
}

GamepadInputEvent GamepadInput::checkDPad()
{
    // Check all D-pad directions
    GamepadInputEvent result;

    result = checkButton(GenericController::BTN_DPAD_UP, INPUT_UP);
    if (result != INPUT_NONE)
        return result;

    result = checkButton(GenericController::BTN_DPAD_DOWN, INPUT_DOWN);
    if (result != INPUT_NONE)
        return result;

    result = checkButton(GenericController::BTN_DPAD_LEFT, INPUT_LEFT);
    if (result != INPUT_NONE)
        return result;

    result = checkButton(GenericController::BTN_DPAD_RIGHT, INPUT_RIGHT);
    if (result != INPUT_NONE)
        return result;

    return INPUT_NONE;
}

GamepadInputEvent GamepadInput::getEvent()
{
    if (!joystick->available())
    {
        return INPUT_NONE;
    }

    // Check D-pad first (for navigation)
    GamepadInputEvent dpadEvent = checkDPad();
    if (dpadEvent != INPUT_NONE)
    {
        return dpadEvent;
    }

    // Check action buttons
    GamepadInputEvent result;

    result = checkButton(GenericController::BTN_SOUTH, INPUT_CONFIRM);
    if (result != INPUT_NONE)
        return result;

    result = checkButton(GenericController::BTN_EAST, INPUT_CANCEL);
    if (result != INPUT_NONE)
        return result;

    result = checkButton(GenericController::BTN_MENU, INPUT_MENU);
    if (result != INPUT_NONE)
        return result;

    result = checkButton(GenericController::BTN_START, INPUT_START);
    if (result != INPUT_NONE)
        return result;

    result = checkButton(GenericController::BTN_SELECT, INPUT_SELECT);
    if (result != INPUT_NONE)
        return result;

    return INPUT_NONE;
}

bool GamepadInput::isHeld(uint8_t genericButton)
{
    return isButtonPressed(genericButton);
}