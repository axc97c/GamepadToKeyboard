#include "mapping/joystick_mappings.h"

int JoystickMapping::mapButtonToGeneric(JoystickController::joytype_t type, uint8_t controllerButton) {
    switch(type) {
        case JoystickController::XBOX360:
        case JoystickController::XBOXONE:
            switch(controllerButton) {
                case Xbox360Physical::A: return GenericController::BTN_SOUTH;
                case Xbox360Physical::B: return GenericController::BTN_EAST;
                case Xbox360Physical::X: return GenericController::BTN_WEST;
                case Xbox360Physical::Y: return GenericController::BTN_NORTH;
                case Xbox360Physical::LB: return GenericController::BTN_L1;
                case Xbox360Physical::RB: return GenericController::BTN_R1;
                case Xbox360Physical::BACK: return GenericController::BTN_SELECT;
                case Xbox360Physical::START: return GenericController::BTN_START;
                case Xbox360Physical::XBOX_BUTTON: return GenericController::BTN_MENU;
                case Xbox360Physical::LEFT_STICK: return GenericController::BTN_L3;
                case Xbox360Physical::RIGHT_STICK: return GenericController::BTN_R3;
                case Xbox360Physical::DPAD_UP: return GenericController::BTN_DPAD_UP;
                case Xbox360Physical::DPAD_DOWN: return GenericController::BTN_DPAD_DOWN;
                case Xbox360Physical::DPAD_LEFT: return GenericController::BTN_DPAD_LEFT;
                case Xbox360Physical::DPAD_RIGHT: return GenericController::BTN_DPAD_RIGHT;
                default: return -1;
            }
            
        case JoystickController::PS4:
            switch(controllerButton) {
                case PS4Physical::CROSS: return GenericController::BTN_SOUTH;
                case PS4Physical::CIRCLE: return GenericController::BTN_EAST;
                case PS4Physical::SQUARE: return GenericController::BTN_WEST;
                case PS4Physical::TRIANGLE: return GenericController::BTN_NORTH;
                case PS4Physical::L1: return GenericController::BTN_L1;
                case PS4Physical::R1: return GenericController::BTN_R1;
                case PS4Physical::L2: return GenericController::BTN_L2;
                case PS4Physical::R2: return GenericController::BTN_R2;
                case PS4Physical::SHARE: return GenericController::BTN_SELECT;
                case PS4Physical::OPTIONS: return GenericController::BTN_START;
                case PS4Physical::PS_BUTTON: return GenericController::BTN_MENU;
                case PS4Physical::L3: return GenericController::BTN_L3;
                case PS4Physical::R3: return GenericController::BTN_R3;
                case PS4Physical::TOUCHPAD: return GenericController::BTN_TOUCHPAD;
                default: return -1;
            }


        case JoystickController::PS3:
        case JoystickController::PS3_MOTION:
            // Similar to PS4 if needed
            return -1;
            
        default:
            return -1;
    }
}

int JoystickMapping::mapAxisToGeneric(JoystickController::joytype_t type, uint8_t controllerAxis) {
    switch(type) {
        case JoystickController::PS4:
        case JoystickController::PS3:
        case JoystickController::PS3_MOTION:
            // PS4/PS3 have different axis ordering
            // Physical axes: 0=LX, 1=LY, 2=RX, 3=L2, 4=R2, 5=RY
            switch(controllerAxis) {
                case GenericController::AXIS_LEFT_X: return 0;
                case GenericController::AXIS_LEFT_Y: return 1;
                case GenericController::AXIS_RIGHT_X: return 2;
                case GenericController::AXIS_RIGHT_Y: return 5;  // PS4 right Y is on axis 5
                case GenericController::AXIS_LEFT_TRIGGER: return 3;
                case GenericController::AXIS_RIGHT_TRIGGER: return 4;
                default: return -1;
            }
            
        case JoystickController::XBOX360:
        case JoystickController::XBOXONE:
        default:
            // Xbox and most controllers use standard ordering
            // 0=LX, 1=LY, 2=RX, 3=RY, 4=LT, 5=RT
            if (controllerAxis <= 5) return controllerAxis;
            return -1;
    }
}

bool JoystickMapping::usesDPadAxis(JoystickController::joytype_t type, uint8_t& axisNumber) {
    switch(type) {
        case JoystickController::PS4:
        case JoystickController::PS3:
        case JoystickController::PS3_MOTION:
            axisNumber = PS4Physical::DPAD_AXIS;
            return true;
        default:
            return false;
    }
}

int JoystickMapping::mapDPadValueToButton(JoystickController::joytype_t type, uint8_t axisValue) {
    switch(type) {
        case JoystickController::PS4:
        case JoystickController::PS3:
        case JoystickController::PS3_MOTION:
            switch(axisValue) {
                case PS4Physical::DPAD_UP_VALUE: return GenericController::BTN_DPAD_UP;
                case PS4Physical::DPAD_DOWN_VALUE: return GenericController::BTN_DPAD_DOWN;
                case PS4Physical::DPAD_LEFT_VALUE: return GenericController::BTN_DPAD_LEFT;
                case PS4Physical::DPAD_RIGHT_VALUE: return GenericController::BTN_DPAD_RIGHT;
                default: return -1;
            }
        default:
            return -1;
    }
}

const char* JoystickMapping::getGenericButtonName(uint8_t genericButton) {
    switch(genericButton) {
        case GenericController::BTN_SOUTH: return "BTN_SOUTH";
        case GenericController::BTN_EAST: return "BTN_EAST";
        case GenericController::BTN_WEST: return "BTN_WEST";
        case GenericController::BTN_NORTH: return "BTN_NORTH";
        case GenericController::BTN_L1: return "BTN_L1";
        case GenericController::BTN_R1: return "BTN_R1";
        case GenericController::BTN_L2: return "BTN_L2";
        case GenericController::BTN_R2: return "BTN_R2";
        case GenericController::BTN_SELECT: return "BTN_SELECT";
        case GenericController::BTN_START: return "BTN_START";
        case GenericController::BTN_MENU: return "BTN_MENU";
        case GenericController::BTN_L3: return "BTN_L3";
        case GenericController::BTN_R3: return "BTN_R3";
        case GenericController::BTN_DPAD_UP: return "BTN_DPAD_UP";
        case GenericController::BTN_DPAD_DOWN: return "BTN_DPAD_DOWN";
        case GenericController::BTN_DPAD_LEFT: return "BTN_DPAD_LEFT";
        case GenericController::BTN_DPAD_RIGHT: return "BTN_DPAD_RIGHT";
        case GenericController::BTN_TOUCHPAD: return "BTN_TOUCHPAD";
        default: return "BTN_UNKNOWN";
    }
}

int JoystickMapping::parseGenericButtonName(const char* buttonName) {
    if (strcmp(buttonName, "BTN_SOUTH") == 0) return GenericController::BTN_SOUTH;
    if (strcmp(buttonName, "BTN_EAST") == 0) return GenericController::BTN_EAST;
    if (strcmp(buttonName, "BTN_WEST") == 0) return GenericController::BTN_WEST;
    if (strcmp(buttonName, "BTN_NORTH") == 0) return GenericController::BTN_NORTH;
    if (strcmp(buttonName, "BTN_L1") == 0) return GenericController::BTN_L1;
    if (strcmp(buttonName, "BTN_R1") == 0) return GenericController::BTN_R1;
    if (strcmp(buttonName, "BTN_L2") == 0) return GenericController::BTN_L2;
    if (strcmp(buttonName, "BTN_R2") == 0) return GenericController::BTN_R2;
    if (strcmp(buttonName, "BTN_SELECT") == 0) return GenericController::BTN_SELECT;
    if (strcmp(buttonName, "BTN_START") == 0) return GenericController::BTN_START;
    if (strcmp(buttonName, "BTN_MENU") == 0) return GenericController::BTN_MENU;
    if (strcmp(buttonName, "BTN_L3") == 0) return GenericController::BTN_L3;
    if (strcmp(buttonName, "BTN_R3") == 0) return GenericController::BTN_R3;
    if (strcmp(buttonName, "BTN_DPAD_UP") == 0) return GenericController::BTN_DPAD_UP;
    if (strcmp(buttonName, "BTN_DPAD_DOWN") == 0) return GenericController::BTN_DPAD_DOWN;
    if (strcmp(buttonName, "BTN_DPAD_LEFT") == 0) return GenericController::BTN_DPAD_LEFT;
    if (strcmp(buttonName, "BTN_DPAD_RIGHT") == 0) return GenericController::BTN_DPAD_RIGHT;
    if (strcmp(buttonName, "BTN_TOUCHPAD") == 0) return GenericController::BTN_TOUCHPAD;
    
    Serial.print("Warning: Unknown generic button name: ");
    Serial.println(buttonName);
    return -1;
}
