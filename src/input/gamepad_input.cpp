#include "input/gamepad_input.h"

GamepadInput::GamepadInput(JoystickController *controller)
{
    joystick = controller;
    reset();
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
}

bool GamepadInput::isButtonPressed(uint8_t genericButton)
{
    if (!joystick->available())
        return false;

    uint32_t buttons = joystick->getButtons();
    JoystickController::joytype_t type = joystick->joystickType();

    int physicalButton = JoystickMapping::mapGenericToButton(type, genericButton);

    if (physicalButton < 0)
    {
        return false;
    }

    return buttons & (1 << physicalButton);
}

bool GamepadInput::isDPadPressed(uint8_t dpadButton)
{
    if (!joystick->available())
        return false;

    JoystickController::joytype_t type = joystick->joystickType();
    uint8_t axisNumber;

    if (JoystickMapping::usesDPadAxis(type, axisNumber))
    {
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
        buttonPressTime[genericButton] = 0;
        buttonRepeating[genericButton] = false;
        return INPUT_NONE;
    }

    unsigned long currentTime = millis();

    if (buttonPressTime[genericButton] == 0)
    {
        buttonPressTime[genericButton] = currentTime;
        return event;
    }

    unsigned long heldTime = currentTime - buttonPressTime[genericButton];

    if (!buttonRepeating[genericButton] && heldTime >= repeatDelay)
    {
        buttonRepeating[genericButton] = true;
        return event;
    }

    if (buttonRepeating[genericButton])
    {
        unsigned long timeSinceRepeatStart = heldTime - repeatDelay;
        if (timeSinceRepeatStart % repeatInterval < 20)
        {
            return event;
        }
    }

    return INPUT_NONE;
}

GamepadInputEvent GamepadInput::checkDPad()
{
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

    GamepadInputEvent dpadEvent = checkDPad();
    if (dpadEvent != INPUT_NONE)
    {
        return dpadEvent;
    }

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