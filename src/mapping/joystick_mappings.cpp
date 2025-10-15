#include "mapping/joystick_mappings.h"

const char* ButtonNames[18] = {
    "A",
    "B",
    "X",
    "Y",
    "L1",
    "R1",
    "L2",
    "R2",
    "Select",
    "Start",
    "Menu",
    "L3",
    "R3",
    "Up",
    "Down",
    "Left",
    "Right",
    "Touchpad"
};

int JoystickMapping::mapButtonToGeneric(JoystickController::joytype_t type, uint8_t controllerButton)
{
    switch (type)
    {
    case JoystickController::XBOX360:
        switch (controllerButton)
        {
        case Xbox360Physical::A:
            return GenericController::BTN_SOUTH;
        case Xbox360Physical::B:
            return GenericController::BTN_EAST;
        case Xbox360Physical::X:
            return GenericController::BTN_WEST;
        case Xbox360Physical::Y:
            return GenericController::BTN_NORTH;
        case Xbox360Physical::LB:
            return GenericController::BTN_L1;
        case Xbox360Physical::RB:
            return GenericController::BTN_R1;
        case Xbox360Physical::BACK:
            return GenericController::BTN_SELECT;
        case Xbox360Physical::START:
            return GenericController::BTN_START;
        case Xbox360Physical::XBOX_BUTTON:
            return GenericController::BTN_MENU;
        case Xbox360Physical::LEFT_STICK:
            return GenericController::BTN_L3;
        case Xbox360Physical::RIGHT_STICK:
            return GenericController::BTN_R3;
        case Xbox360Physical::DPAD_UP:
            return GenericController::BTN_DPAD_UP;
        case Xbox360Physical::DPAD_DOWN:
            return GenericController::BTN_DPAD_DOWN;
        case Xbox360Physical::DPAD_LEFT:
            return GenericController::BTN_DPAD_LEFT;
        case Xbox360Physical::DPAD_RIGHT:
            return GenericController::BTN_DPAD_RIGHT;
        default:
            return -1;
        }

    case JoystickController::PS4:
        switch (controllerButton)
        {
        case PS4Physical::CROSS:
            return GenericController::BTN_SOUTH;
        case PS4Physical::CIRCLE:
            return GenericController::BTN_EAST;
        case PS4Physical::SQUARE:
            return GenericController::BTN_WEST;
        case PS4Physical::TRIANGLE:
            return GenericController::BTN_NORTH;
        case PS4Physical::L1:
            return GenericController::BTN_L1;
        case PS4Physical::R1:
            return GenericController::BTN_R1;
        case PS4Physical::L2:
            return GenericController::BTN_L2;
        case PS4Physical::R2:
            return GenericController::BTN_R2;
        case PS4Physical::SHARE:
            return GenericController::BTN_SELECT;
        case PS4Physical::OPTIONS:
            return GenericController::BTN_START;
        case PS4Physical::PS_BUTTON:
            return GenericController::BTN_MENU;
        case PS4Physical::L3:
            return GenericController::BTN_L3;
        case PS4Physical::R3:
            return GenericController::BTN_R3;
        case PS4Physical::TOUCHPAD:
            return GenericController::BTN_TOUCHPAD;
        default:
            return -1;
        }

    default:
        return -1;
    }
}

int JoystickMapping::mapGenericToButton(JoystickController::joytype_t type, uint8_t genericButton)
{
    switch (type)
    {
    case JoystickController::XBOX360:
        switch (genericButton)
        {
        case GenericController::BTN_SOUTH:
            return Xbox360Physical::A;
        case GenericController::BTN_EAST:
            return Xbox360Physical::B;
        case GenericController::BTN_WEST:
            return Xbox360Physical::X;
        case GenericController::BTN_NORTH:
            return Xbox360Physical::Y;
        case GenericController::BTN_L1:
            return Xbox360Physical::LB;
        case GenericController::BTN_R1:
            return Xbox360Physical::RB;
        case GenericController::BTN_SELECT:
            return Xbox360Physical::BACK;
        case GenericController::BTN_START:
            return Xbox360Physical::START;
        case GenericController::BTN_MENU:
            return Xbox360Physical::XBOX_BUTTON;
        case GenericController::BTN_L3:
            return Xbox360Physical::LEFT_STICK;
        case GenericController::BTN_R3:
            return Xbox360Physical::RIGHT_STICK;
        case GenericController::BTN_DPAD_UP:
            return Xbox360Physical::DPAD_UP;
        case GenericController::BTN_DPAD_DOWN:
            return Xbox360Physical::DPAD_DOWN;
        case GenericController::BTN_DPAD_LEFT:
            return Xbox360Physical::DPAD_LEFT;
        case GenericController::BTN_DPAD_RIGHT:
            return Xbox360Physical::DPAD_RIGHT;
        default:
            return -1;
        }

    case JoystickController::PS4:
        switch (genericButton)
        {
        case GenericController::BTN_SOUTH:
            return PS4Physical::CROSS;
        case GenericController::BTN_EAST:
            return PS4Physical::CIRCLE;
        case GenericController::BTN_WEST:
            return PS4Physical::SQUARE;
        case GenericController::BTN_NORTH:
            return PS4Physical::TRIANGLE;
        case GenericController::BTN_L1:
            return PS4Physical::L1;
        case GenericController::BTN_R1:
            return PS4Physical::R1;
        case GenericController::BTN_L2:
            return PS4Physical::L2;
        case GenericController::BTN_R2:
            return PS4Physical::R2;
        case GenericController::BTN_SELECT:
            return PS4Physical::SHARE;
        case GenericController::BTN_START:
            return PS4Physical::OPTIONS;
        case GenericController::BTN_MENU:
            return PS4Physical::PS_BUTTON;
        case GenericController::BTN_L3:
            return PS4Physical::L3;
        case GenericController::BTN_R3:
            return PS4Physical::R3;
        case GenericController::BTN_TOUCHPAD:
            return PS4Physical::TOUCHPAD;
        // D-pad buttons not directly mappable for PS4 (uses axis)
        case GenericController::BTN_DPAD_UP:
        case GenericController::BTN_DPAD_DOWN:
        case GenericController::BTN_DPAD_LEFT:
        case GenericController::BTN_DPAD_RIGHT:
            return -1; // D-pad is axis-based on PS4
        default:
            return -1;
        }

    default:
        return -1;
    }
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
    if (genericButton <= GenericController::BTN_TOUCHPAD)
    {
        return ButtonNames[genericButton];
    }

    return "Unknown";
}

int JoystickMapping::parseGenericButtonName(const char *buttonName)
{
    for (int i = 0 ; i <= GenericController::BTN_TOUCHPAD ; i++)
    {
        const char * name = ButtonNames[i];
        if (stricmp(buttonName, "BTN_SOUTH") == 0)
        {
            return i;    
        } 
    }

    Serial.print("Warning: Unknown generic button name: ");
    Serial.println(buttonName);
    return -1;
}
