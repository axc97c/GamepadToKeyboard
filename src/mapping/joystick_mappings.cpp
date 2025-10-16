#include "mapping/joystick_mappings.h"

// Static button name mapping array
const JoystickMapping::ButtonNameMapping JoystickMapping::buttonNameMap[] = {
    {GenericController::BTN_SOUTH, "A"},
    {GenericController::BTN_EAST, "B"},
    {GenericController::BTN_WEST, "X"},
    {GenericController::BTN_NORTH, "Y"},
    {GenericController::BTN_L1, "L1"},
    {GenericController::BTN_R1, "R1"},
    {GenericController::BTN_L2, "L2"},
    {GenericController::BTN_R2, "R2"},
    {GenericController::BTN_SELECT, "Select"},
    {GenericController::BTN_START, "Start"},
    {GenericController::BTN_MENU, "Menu"},
    {GenericController::BTN_L3, "L3"},
    {GenericController::BTN_R3, "R3"},
    {GenericController::BTN_DPAD_UP, "Up"},
    {GenericController::BTN_DPAD_DOWN, "Down"},
    {GenericController::BTN_DPAD_LEFT, "Left"},
    {GenericController::BTN_DPAD_RIGHT, "Right"},
    {GenericController::BTN_TOUCHPAD, "Touchpad"}
};

const int JoystickMapping::buttonNameMapSize = sizeof(JoystickMapping::buttonNameMap) / sizeof(JoystickMapping::buttonNameMap[0]);

// Xbox 360 button mapping array
const JoystickMapping::ControllerButtonMapping JoystickMapping::xbox360ButtonMap[] = {
    {Xbox360Physical::A, GenericController::BTN_SOUTH},
    {Xbox360Physical::B, GenericController::BTN_EAST},
    {Xbox360Physical::X, GenericController::BTN_WEST},
    {Xbox360Physical::Y, GenericController::BTN_NORTH},
    {Xbox360Physical::LB, GenericController::BTN_L1},
    {Xbox360Physical::RB, GenericController::BTN_R1},
    {Xbox360Physical::BACK, GenericController::BTN_SELECT},
    {Xbox360Physical::START, GenericController::BTN_START},
    {Xbox360Physical::XBOX_BUTTON, GenericController::BTN_MENU},
    {Xbox360Physical::LEFT_STICK, GenericController::BTN_L3},
    {Xbox360Physical::RIGHT_STICK, GenericController::BTN_R3},
    {Xbox360Physical::DPAD_UP, GenericController::BTN_DPAD_UP},
    {Xbox360Physical::DPAD_DOWN, GenericController::BTN_DPAD_DOWN},
    {Xbox360Physical::DPAD_LEFT, GenericController::BTN_DPAD_LEFT},
    {Xbox360Physical::DPAD_RIGHT, GenericController::BTN_DPAD_RIGHT}
};

const int JoystickMapping::xbox360ButtonMapSize = sizeof(JoystickMapping::xbox360ButtonMap) / sizeof(JoystickMapping::xbox360ButtonMap[0]);

// PS4 button mapping array
const JoystickMapping::ControllerButtonMapping JoystickMapping::ps4ButtonMap[] = {
    {PS4Physical::CROSS, GenericController::BTN_SOUTH},
    {PS4Physical::CIRCLE, GenericController::BTN_EAST},
    {PS4Physical::SQUARE, GenericController::BTN_WEST},
    {PS4Physical::TRIANGLE, GenericController::BTN_NORTH},
    {PS4Physical::L1, GenericController::BTN_L1},
    {PS4Physical::R1, GenericController::BTN_R1},
    {PS4Physical::L2, GenericController::BTN_L2},
    {PS4Physical::R2, GenericController::BTN_R2},
    {PS4Physical::SHARE, GenericController::BTN_SELECT},
    {PS4Physical::OPTIONS, GenericController::BTN_START},
    {PS4Physical::PS_BUTTON, GenericController::BTN_MENU},
    {PS4Physical::L3, GenericController::BTN_L3},
    {PS4Physical::R3, GenericController::BTN_R3},
    {PS4Physical::TOUCHPAD, GenericController::BTN_TOUCHPAD}
};

const int JoystickMapping::ps4ButtonMapSize = sizeof(JoystickMapping::ps4ButtonMap) / sizeof(JoystickMapping::ps4ButtonMap[0]);

int JoystickMapping::mapButtonToGeneric(JoystickController::joytype_t type, uint8_t controllerButton)
{
    const ControllerButtonMapping *mappingArray = nullptr;
    int mappingSize = 0;

    switch (type)
    {
    case JoystickController::XBOX360:
        mappingArray = xbox360ButtonMap;
        mappingSize = xbox360ButtonMapSize;
        break;
    case JoystickController::PS4:
        mappingArray = ps4ButtonMap;
        mappingSize = ps4ButtonMapSize;
        break;
    default:
        return -1;
    }

    for (int i = 0; i < mappingSize; i++)
    {
        if (mappingArray[i].physicalButton == controllerButton)
        {
            return mappingArray[i].genericButton;
        }
    }

    return -1;
}

int JoystickMapping::mapGenericToButton(JoystickController::joytype_t type, uint8_t genericButton)
{
    const ControllerButtonMapping *mappingArray = nullptr;
    int mappingSize = 0;

    switch (type)
    {
    case JoystickController::XBOX360:
        mappingArray = xbox360ButtonMap;
        mappingSize = xbox360ButtonMapSize;
        break;
    case JoystickController::PS4:
        mappingArray = ps4ButtonMap;
        mappingSize = ps4ButtonMapSize;
        break;
    default:
        return -1;
    }

    for (int i = 0; i < mappingSize; i++)
    {
        if (mappingArray[i].genericButton == genericButton)
        {
            return mappingArray[i].physicalButton;
        }
    }

    return -1;
}

int JoystickMapping::mapAxisToGeneric(JoystickController::joytype_t type, uint8_t controllerAxis)
{
    switch (type)
    {
    case JoystickController::PS4:
        // PS4/PS3 have different axis ordering
        // Physical axes: 0=LX, 1=LY, 2=RX, 3=L2, 4=R2, 5=RY
        switch (controllerAxis)
        {
        case GenericController::AXIS_LEFT_X:
            return 0;
        case GenericController::AXIS_LEFT_Y:
            return 1;
        case GenericController::AXIS_RIGHT_X:
            return 2;
        case GenericController::AXIS_RIGHT_Y:
            return 5; // PS4 right Y is on axis 5
        case GenericController::AXIS_LEFT_TRIGGER:
            return 3;
        case GenericController::AXIS_RIGHT_TRIGGER:
            return 4;
        default:
            return -1;
        }

    case JoystickController::XBOX360:
    default:
        // Xbox and most controllers use standard ordering
        // 0=LX, 1=LY, 2=RX, 3=RY, 4=LT, 5=RT
        if (controllerAxis <= 5)
            return controllerAxis;
        return -1;
    }
}

bool JoystickMapping::usesDPadAxis(JoystickController::joytype_t type, uint8_t &axisNumber)
{
    switch (type)
    {
    case JoystickController::PS4:
        axisNumber = PS4Physical::DPAD_AXIS;
        return true;
    default:
        return false;
    }
}

int JoystickMapping::mapDPadValueToButton(JoystickController::joytype_t type, uint8_t axisValue)
{
    switch (type)
    {
    case JoystickController::PS4:
        switch (axisValue)
        {
        case PS4Physical::DPAD_UP_VALUE:
            return GenericController::BTN_DPAD_UP;
        case PS4Physical::DPAD_DOWN_VALUE:
            return GenericController::BTN_DPAD_DOWN;
        case PS4Physical::DPAD_LEFT_VALUE:
            return GenericController::BTN_DPAD_LEFT;
        case PS4Physical::DPAD_RIGHT_VALUE:
            return GenericController::BTN_DPAD_RIGHT;
        default:
            return -1;
        }
    default:
        return -1;
    }
}

const char *JoystickMapping::getGenericButtonName(uint8_t genericButton)
{
    for (int i = 0; i < buttonNameMapSize; i++)
    {
        if (buttonNameMap[i].button == genericButton)
        {
            return buttonNameMap[i].name;
        }
    }

    return "Unknown";
}

int JoystickMapping::parseGenericButtonName(const char *buttonName)
{
    for (int i = 0; i < buttonNameMapSize; i++)
    {
        if (strcasecmp(buttonName, buttonNameMap[i].name) == 0)
        {
            return buttonNameMap[i].button;
        }
    }

    Serial.print("JoystickMapping: Warning: Unknown generic button name: ");
    Serial.println(buttonName);
    return -1;
}