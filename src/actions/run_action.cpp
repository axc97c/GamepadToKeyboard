#include "actions/run_action.h"
#include "mapping/mapping_config.h"
#include "devices.h"
#include <Keyboard.h>
#include <Mouse.h>
#include <USBHost_t36.h>

RunAction::RunAction(DeviceManager *dev, ActionHandler *hdlr, RunActionParams p)
    : Action(dev, hdlr),
      params(p),
      numMappings(0),
      controllerType(JoystickController::UNKNOWN),
      lastStickUpdate(0),
      stickUpdateInterval(1000 / 60),
      lastDPadAxisValue(-1)
{
}

void RunAction::init()
{
    Serial.println("RunAction: Initialized");

    Mouse.begin();

    // Detect controller type
    JoystickController *joy = devices->getJoystick();
    if (joy && *joy)
    {
        controllerType = joy->joystickType();
        Serial.print("Controller type detected: ");
        Serial.println(controllerType);
    }

    // Try to load button mappings
    if (!MappingConfig::loadConfig("/config.json", mappings, numMappings, MAX_MAPPINGS, &leftStick, &rightStick))
    {
        Serial.println("Failed to load button mappings, using defaults");
        initializeDefaultMappings();
        initializeDefaultStickConfigs();

        MappingConfig::saveConfig("/config.json", mappings, numMappings, &leftStick, &rightStick);
    }

    Serial.println("RunAction initialization complete");
}

void RunAction::loop()
{
    JoystickController *joy = devices->getJoystick();

    if (joy && *joy)
    {
        // Update controller type if changed
        JoystickController::joytype_t currentType = joy->joystickType();
        if (currentType != controllerType)
        {
            controllerType = currentType;
            Serial.print("Controller type changed to: ");
            Serial.println(controllerType);
        }

        // Process button mappings
        processButtonMappings();

        // Process D-pad if controller uses axis-based D-pad
        processDPadAxisMappings();

        // Process analog sticks
        int leftXAxis = JoystickMapping::mapAxisToGeneric(controllerType, GenericController::AXIS_LEFT_X);
        int leftYAxis = JoystickMapping::mapAxisToGeneric(controllerType, GenericController::AXIS_LEFT_Y);
        int rightXAxis = JoystickMapping::mapAxisToGeneric(controllerType, GenericController::AXIS_RIGHT_X);
        int rightYAxis = JoystickMapping::mapAxisToGeneric(controllerType, GenericController::AXIS_RIGHT_Y);

        if (leftXAxis != -1 && leftYAxis != -1)
        {
            processAnalogStick(leftStick, leftXAxis, leftYAxis);
        }

        if (rightXAxis != -1 && rightYAxis != -1)
        {
            processAnalogStick(rightStick, rightXAxis, rightYAxis);
        }
    }
}

void RunAction::processButtonMappings()
{
    JoystickController *joy = devices->getJoystick();
    uint32_t buttons = joy->getButtons();

    for (int i = 0; i < numMappings; i++)
    {
        uint8_t genericButton = mappings[i].genericButton;

        // Skip D-pad buttons if controller uses D-pad axis
        uint8_t dpadAxis;
        if (JoystickMapping::usesDPadAxis(controllerType, dpadAxis))
        {
            if (genericButton >= GenericController::BTN_DPAD_UP &&
                genericButton <= GenericController::BTN_DPAD_RIGHT)
            {
                continue; // Handle these in processDPadAxisMappings
            }
        }

        // Find the controller-specific button for this generic button
        bool isPressed = false;

        // Check all physical buttons to find which maps to this generic button
        for (uint8_t physicalBtn = 0; physicalBtn < 32; physicalBtn++)
        {
            int mappedGeneric = JoystickMapping::mapButtonToGeneric(controllerType, physicalBtn);
            if (mappedGeneric == genericButton)
            {
                isPressed = (buttons & (1 << physicalBtn)) != 0;
                break;
            }
        }

        // Detect state change
        if (isPressed != mappings[i].currentlyPressed)
        {
            mappings[i].currentlyPressed = isPressed;

            if (isPressed)
            {
                Serial.print("Button ");
                Serial.print(getGenericButtonName(genericButton));
                Serial.print(" pressed -> Key ");
                Serial.println(mappings[i].keyCode);

                Keyboard.press(mappings[i].keyCode);
            }
            else
            {
                Serial.print("Button ");
                Serial.print(getGenericButtonName(genericButton));
                Serial.println(" released");

                Keyboard.release(mappings[i].keyCode);
            }
        }
    }
}

void RunAction::processDPadAxisMappings()
{
    uint8_t dpadAxis;
    if (!JoystickMapping::usesDPadAxis(controllerType, dpadAxis))
    {
        return; // Controller doesn't use D-pad axis
    }

    JoystickController *joy = devices->getJoystick();
    int axisValue = joy->getAxis(dpadAxis);

    // Only process if value changed
    if (axisValue == lastDPadAxisValue)
    {
        return;
    }
    lastDPadAxisValue = axisValue;

    // Check each D-pad mapping
    for (int i = 0; i < numMappings; i++)
    {
        uint8_t genericButton = mappings[i].genericButton;

        // Only process D-pad buttons
        if (genericButton < GenericController::BTN_DPAD_UP ||
            genericButton > GenericController::BTN_DPAD_RIGHT)
        {
            continue;
        }

        // Check if current axis value corresponds to this D-pad button
        int mappedButton = JoystickMapping::mapDPadValueToButton(controllerType, axisValue);
        bool isPressed = (mappedButton == genericButton);

        // Detect state change
        if (isPressed != mappings[i].currentlyPressed)
        {
            mappings[i].currentlyPressed = isPressed;

            if (isPressed)
            {
                Serial.print("D-Pad ");
                Serial.print(getGenericButtonName(genericButton));
                Serial.print(" pressed -> Key ");
                Serial.println(mappings[i].keyCode);

                Keyboard.press(mappings[i].keyCode);
            }
            else
            {
                Serial.print("D-Pad ");
                Serial.print(getGenericButtonName(genericButton));
                Serial.println(" released");

                Keyboard.release(mappings[i].keyCode);
            }
        }
    }
}

void RunAction::processAnalogStick(StickConfig &stick, int xAxis, int yAxis)
{
    if (stick.behavior == StickBehavior::DISABLED)
    {
        return;
    }

    JoystickController *joy = devices->getJoystick();
    int xValue = joy->getAxis(xAxis);
    int yValue = joy->getAxis(yAxis);

    // Apply behavior
    switch (stick.behavior)
    {
    case StickBehavior::MOUSE_MOVEMENT:
        processMouseMovement(stick, xValue, yValue);
        break;

    case StickBehavior::BUTTON_EMULATION:
        processButtonEmulation(stick, xValue, yValue);
        break;

    case StickBehavior::SCROLL_WHEEL:
        processScrollWheel(stick, yValue);
        break;

    case StickBehavior::WASD_KEYS:
        processWASDKeys(stick, xValue, yValue);
        break;

    case StickBehavior::ARROW_KEYS:
        processArrowKeys(stick, xValue, yValue);
        break;

    default:
        break;
    }
}

void RunAction::processMouseMovement(StickConfig &stick, int xValue, int yValue)
{
    unsigned long currentTime = millis();
    if (currentTime - lastStickUpdate < stickUpdateInterval)
    {
        return;
    }
    lastStickUpdate = currentTime;

    int adjustedX = applyDeadzone(xValue, 128, stick.deadzone);
    int adjustedY = applyDeadzone(yValue, 128, stick.deadzone);

    if (adjustedX != 0 || adjustedY != 0)
    {
        int mouseX = (int)(adjustedX * stick.sensitivity);
        int mouseY = (int)(adjustedY * stick.sensitivity);

        // Serial.print("Mouse ");
        // Serial.print(xValue);
        // Serial.print(", ");
        // Serial.print(yValue);
        // Serial.print(" - ");
        // Serial.print(adjustedX);
        // Serial.print(", ");
        // Serial.print(adjustedY);
        // Serial.print(" - ");
        // Serial.print(mouseX);
        // Serial.print(", ");
        // Serial.println(mouseY);

        Mouse.move(mouseX, mouseY, 0);
    }
}

void RunAction::processButtonEmulation(StickConfig &stick, int xValue, int yValue)
{
    int adjustedX = applyDeadzone(xValue, 128, stick.deadzone);
    int adjustedY = applyDeadzone(yValue, 128, stick.deadzone);

    // Check each direction
    bool shouldBeUpPressed = (adjustedY < -stick.activationThreshold);
    bool shouldBeDownPressed = (adjustedY > stick.activationThreshold);
    bool shouldBeLeftPressed = (adjustedX < -stick.activationThreshold);
    bool shouldBeRightPressed = (adjustedX > stick.activationThreshold);

    // Up
    if (shouldBeUpPressed && !stick.upPressed)
    {
        Keyboard.press(stick.keyUp);
        stick.upPressed = true;
    }
    else if (!shouldBeUpPressed && stick.upPressed)
    {
        Keyboard.release(stick.keyUp);
        stick.upPressed = false;
    }

    // Down
    if (shouldBeDownPressed && !stick.downPressed)
    {
        Keyboard.press(stick.keyDown);
        stick.downPressed = true;
    }
    else if (!shouldBeDownPressed && stick.downPressed)
    {
        Keyboard.release(stick.keyDown);
        stick.downPressed = false;
    }

    // Left
    if (shouldBeLeftPressed && !stick.leftPressed)
    {
        Keyboard.press(stick.keyLeft);
        stick.leftPressed = true;
    }
    else if (!shouldBeLeftPressed && stick.leftPressed)
    {
        Keyboard.release(stick.keyLeft);
        stick.leftPressed = false;
    }

    // Right
    if (shouldBeRightPressed && !stick.rightPressed)
    {
        Keyboard.press(stick.keyRight);
        stick.rightPressed = true;
    }
    else if (!shouldBeRightPressed && stick.rightPressed)
    {
        Keyboard.release(stick.keyRight);
        stick.rightPressed = false;
    }
}

void RunAction::processScrollWheel(StickConfig &stick, int yValue)
{
    unsigned long currentTime = millis();
    if (currentTime - lastStickUpdate < stickUpdateInterval)
    {
        return;
    }
    lastStickUpdate = currentTime;

    int adjustedY = applyDeadzone(yValue, 128, stick.deadzone);

    if (adjustedY != 0)
    {
        int scroll = (int)(adjustedY * stick.sensitivity * 0.1f);
        if (scroll != 0)
        {
            Mouse.move(0, 0, -scroll); // Negative for natural scrolling
        }
    }
}

void RunAction::processWASDKeys(StickConfig &stick, int xValue, int yValue)
{
    stick.keyUp = 'w';
    stick.keyDown = 's';
    stick.keyLeft = 'a';
    stick.keyRight = 'd';
    processButtonEmulation(stick, xValue, yValue);
}

void RunAction::processArrowKeys(StickConfig &stick, int xValue, int yValue)
{
    stick.keyUp = KEY_UP;
    stick.keyDown = KEY_DOWN;
    stick.keyLeft = KEY_LEFT;
    stick.keyRight = KEY_RIGHT;
    processButtonEmulation(stick, xValue, yValue);
}

int RunAction::applyDeadzone(int value, int centerValue, int deadzone)
{
    int centered = value - centerValue;

    if (abs(centered) < deadzone)
    {
        return 0;
    }

    return centered;
}

void RunAction::initializeDefaultMappings()
{
    numMappings = 0;

    // Face buttons - WASD
    mappings[numMappings++] = {GenericController::BTN_NORTH, 'w', false};
    mappings[numMappings++] = {GenericController::BTN_SOUTH, 's', false};
    mappings[numMappings++] = {GenericController::BTN_WEST, 'a', false};
    mappings[numMappings++] = {GenericController::BTN_EAST, 'd', false};

    // D-Pad - Arrow keys
    mappings[numMappings++] = {GenericController::BTN_DPAD_UP, KEY_UP, false};
    mappings[numMappings++] = {GenericController::BTN_DPAD_DOWN, KEY_DOWN, false};
    mappings[numMappings++] = {GenericController::BTN_DPAD_LEFT, KEY_LEFT, false};
    mappings[numMappings++] = {GenericController::BTN_DPAD_RIGHT, KEY_RIGHT, false};

    // Shoulder buttons
    mappings[numMappings++] = {GenericController::BTN_L1, 'q', false};
    mappings[numMappings++] = {GenericController::BTN_R1, 'e', false};

    // Start/Select
    mappings[numMappings++] = {GenericController::BTN_START, 'r', false};
    mappings[numMappings++] = {GenericController::BTN_SELECT, 'f', false};

    // Stick clicks
    mappings[numMappings++] = {GenericController::BTN_L3, 't', false};
    mappings[numMappings++] = {GenericController::BTN_R3, 'g', false};

    // Menu button
    mappings[numMappings++] = {GenericController::BTN_MENU, 'y', false};
    mappings[numMappings++] = {GenericController::BTN_TOUCHPAD, 'h', false};

    Serial.print("Initialized ");
    Serial.print(numMappings);
    Serial.println(" default generic mappings");
}

void RunAction::initializeDefaultStickConfigs()
{
    rightStick.behavior = StickBehavior::MOUSE_MOVEMENT;
    leftStick.behavior = StickBehavior::ARROW_KEYS;

    Serial.println("Default stick configurations initialized");
}

const char *RunAction::getGenericButtonName(uint8_t genericButton)
{
    return JoystickMapping::getGenericButtonName(genericButton);
}

ActionType RunAction::getType()
{
    return ActionType::RUN;
}

RunActionParams &RunAction::getParams()
{
    return params;
}