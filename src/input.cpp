#include "input.h"

Input::Input(JoystickController *controller)
{
    joystick = controller;
    lastButtons = 0;
    lastDPadValue = 0xFF;
    dPadUsesAxis = false;
    dPadAxisNumber = 0;

    // Initialize button state arrays
    for (int i = 0; i < 32; i++)
    {
        buttonPressTime[i] = 0;
        buttonRepeating[i] = false;
    }
}

void Input::setup()
{
    // Determine if this controller uses D-pad as axis
    if (joystick->joystickType() != JoystickController::UNKNOWN)
    {
        dPadUsesAxis = JoystickMapping::usesDPadAxis(joystick->joystickType(), dPadAxisNumber);
    }
    else
    {
        Serial.println("Unknown joystick type");
    }

    Serial.println("Input setup complete");
    if (dPadUsesAxis)
    {
        Serial.print("Controller uses D-pad axis: ");
        Serial.println(dPadAxisNumber);
    }
}

void Input::update()
{
    // This method can be used for any per-frame updates if needed
    // Currently, all logic is in getEvent()
}

void Input::reset()
{
    lastButtons = 0;
    lastDPadValue = 0xFF;

    for (int i = 0; i < 32; i++)
    {
        buttonPressTime[i] = 0;
        buttonRepeating[i] = false;
    }

    Serial.println("Input state reset");
}

bool Input::isButtonPressed(uint8_t genericButton)
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

bool Input::isDPadPressed(uint8_t dpadButton)
{
    if (!joystick->available())
        return false;

    if (dPadUsesAxis)
    {
        // Read D-pad from axis
        uint64_t axisMask = joystick->axisMask();
        if (axisMask & (1 << dPadAxisNumber))
        {
            int axisValue = joystick->getAxis(dPadAxisNumber);
            int mappedButton = JoystickMapping::mapDPadValueToButton(
                joystick->joystickType(),
                axisValue);
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

InputEvent Input::checkButton(uint8_t genericButton, InputEvent event)
{
    if (!isButtonPressed(genericButton))
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

InputEvent Input::checkDPad()
{
    // Check all D-pad directions
    InputEvent result;

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

InputEvent Input::getEvent()
{
    if (!joystick->available())
    {
        return INPUT_NONE;
    }

    // Check D-pad first (for navigation)
    InputEvent dpadEvent = checkDPad();
    if (dpadEvent != INPUT_NONE)
    {
        return dpadEvent;
    }

    // Check action buttons
    InputEvent result;

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

bool Input::isHeld(uint8_t genericButton)
{
    return isButtonPressed(genericButton);
}