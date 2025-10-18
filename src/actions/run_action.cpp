#include "actions/run_action.h"
#include "mapping/mapping_config.h"
#include "actions/action_handler.h"
#include "devices.h"
#include <Keyboard.h>
#include <Mouse.h>
#include <USBHost_t36.h>
#include "utils.h"

RunAction::RunAction(DeviceManager *dev, ActionHandler *hdlr, RunActionParams p)
    : Action(dev, hdlr),
      params(p),
      controllerType(JoystickController::UNKNOWN),
      lastStickUpdate(0),
      stickUpdateInterval(1000 / 60),
      lastDPadAxisValue(-1)
{
}

void RunAction::init()
{
    Serial.println("RunAction: Initialized");

    // Set up keyboard passthrough event handlers
    // KeyboardController *kbd = devices->getKeyboard();
    // if (kbd)
    // {
    //     kbd->attachPress(onKeyPress);
    //     kbd->attachRelease(onKeyRelease);
    //     kbd->attachExtrasPress(onExtrasPress);
    //     kbd->attachExtrasRelease(onExtrasRelease);
    //     Serial.println("Keyboard passthrough handlers attached");
    // }

    // Detect controller type
    JoystickController *joy = devices->getJoystick();
    if (joy && *joy)
    {
        controllerType = joy->joystickType();
        Serial.print("RunAction: Controller type detected: ");
        Serial.println(controllerType);
    }

    Serial.print("RunAction: params.filename = ");
    Serial.println(params.filename);

    if (params.filename[0] != '\0')
    {
        if (!MappingConfig::loadConfig(params.filename, mappingConfig))
        {
            Serial.println("RunAction: Failed to load button mappings, using defaults");
            initializeDefaultMappings();
            initializeDefaultStickConfigs();
            initializeDefaultTriggerConfigs();

            MappingConfig::saveConfig(params.filename, mappingConfig);
        }

        // Clear loading filename
        params.filename[0] = '\0';
    }

    DisplayLoadedFile();
    Serial.println("RunAction: RunAction initialization complete");
}

void RunAction::loop()
{
    if (backlightOnTime > 0)
    {
        unsigned long currentTime = millis();
        if (currentTime > backlightOnTime + BACKLIGHT_TIMEOUT_MS)
        {
            Serial.println("RunAction: Backlight off");
            devices->getLCD()->noBacklight();
            backlightOnTime = 0;
        }
    }

    JoystickController *joy = devices->getJoystick();

    if (joy && *joy)
    {
        // Update controller type if changed
        JoystickController::joytype_t currentType = joy->joystickType();
        if (currentType != controllerType)
        {
            controllerType = currentType;
            Serial.print("RunAction: Controller type changed to: ");
            Serial.println(controllerType);
        }

        // Check for menu button press (Xbox/PS button)
        uint32_t buttons = joy->getButtons();
        for (uint8_t physicalBtn = 0; physicalBtn < 32; physicalBtn++)
        {
            int mappedGeneric = JoystickMapping::mapButtonToGeneric(controllerType, physicalBtn);
            if (mappedGeneric == GenericController::BTN_MENU)
            {
                bool isPressed = (buttons & (1 << physicalBtn)) != 0;
                if (isPressed)
                {
                    handler->activateMainMenu();
                    return;
                }
                break;
            }
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
        int leftTriggerAxis = JoystickMapping::mapAxisToGeneric(controllerType, GenericController::AXIS_LEFT_TRIGGER);
        int rightTriggerAxis = JoystickMapping::mapAxisToGeneric(controllerType, GenericController::AXIS_RIGHT_TRIGGER);

        if (leftXAxis != -1 && leftYAxis != -1)
        {
            processAnalogStick(mappingConfig.leftStick, leftXAxis, leftYAxis);
        }

        if (rightXAxis != -1 && rightYAxis != -1)
        {
            processAnalogStick(mappingConfig.rightStick, rightXAxis, rightYAxis);
        }

        if (leftTriggerAxis != -1 && rightTriggerAxis != -1)
        {
            processTriggers(leftTriggerAxis, rightTriggerAxis);
        }
    }
}

void RunAction::processButtonMappings()
{
    JoystickController *joy = devices->getJoystick();
    uint32_t buttons = joy->getButtons();

    for (int i = 0; i < mappingConfig.numMappings; i++)
    {
        uint8_t genericButton = mappingConfig.mappings[i].genericButton;

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
        if (isPressed != mappingConfig.mappings[i].currentlyPressed)
        {
            mappingConfig.mappings[i].currentlyPressed = isPressed;

            if (isPressed)
            {
                Serial.print("RunAction: Button ");
                Serial.print(getGenericButtonName(genericButton));
                Serial.print(" pressed -> Key ");
                Serial.println(mappingConfig.mappings[i].keyCode);

                Keyboard.press(mappingConfig.mappings[i].keyCode);
            }
            else
            {
                Serial.print("RunAction: Button ");
                Serial.print(getGenericButtonName(genericButton));
                Serial.println(" released");

                Keyboard.release(mappingConfig.mappings[i].keyCode);
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
    for (int i = 0; i < mappingConfig.numMappings; i++)
    {
        uint8_t genericButton = mappingConfig.mappings[i].genericButton;

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
        if (isPressed != mappingConfig.mappings[i].currentlyPressed)
        {
            mappingConfig.mappings[i].currentlyPressed = isPressed;

            if (isPressed)
            {
                Serial.print("RunAction: D-Pad ");
                Serial.print(getGenericButtonName(genericButton));
                Serial.print(" pressed -> Key ");
                Serial.println(mappingConfig.mappings[i].keyCode);

                Keyboard.press(mappingConfig.mappings[i].keyCode);
            }
            else
            {
                Serial.print("RunAction: D-Pad ");
                Serial.print(getGenericButtonName(genericButton));
                Serial.println(" released");

                Keyboard.release(mappingConfig.mappings[i].keyCode);
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

void RunAction::processTriggers(int leftAxis, int rightAxis)
{
    if (mappingConfig.triggers.behavior == TriggerBehavior::DISABLED)
    {
        return;
    }

    JoystickController *joy = devices->getJoystick();
    int leftValue = joy->getAxis(leftAxis);
    int rightValue = joy->getAxis(rightAxis);

    // Apply behavior
    switch (mappingConfig.triggers.behavior)
    {
    case TriggerBehavior::BUTTONS:
        processTriggerButtons(leftValue, rightValue);
        break;

    case TriggerBehavior::MOUSE_X:
        processTriggerMouseX(leftValue, rightValue);
        break;

    case TriggerBehavior::MOUSE_Y:
        processTriggerMouseY(leftValue, rightValue);
        break;

    case TriggerBehavior::SCROLL_WHEEL:
        processTriggerScroll(leftValue, rightValue);
        break;

    case TriggerBehavior::JOYSTICK_X:
    case TriggerBehavior::JOYSTICK_Y:
        // Joystick modes pass raw axis data - no processing needed here
        break;

    default:
        break;
    }
}

void RunAction::processTriggerButtons(int leftValue, int rightValue)
{
    // Triggers are 0-255, check against threshold
    bool shouldBeLeftPressed = (leftValue > mappingConfig.triggers.activationThreshold);
    bool shouldBeRightPressed = (rightValue > mappingConfig.triggers.activationThreshold);

    // Left trigger
    if (shouldBeLeftPressed && !mappingConfig.triggers.leftPressed)
    {
        Keyboard.press(mappingConfig.triggers.keyLeft);
        mappingConfig.triggers.leftPressed = true;
        Serial.print("RunAction: Left Trigger pressed -> Key ");
        Serial.println(mappingConfig.triggers.keyLeft);
    }
    else if (!shouldBeLeftPressed && mappingConfig.triggers.leftPressed)
    {
        Keyboard.release(mappingConfig.triggers.keyLeft);
        mappingConfig.triggers.leftPressed = false;
        Serial.println("RunAction: Left Trigger released");
    }

    // Right trigger
    if (shouldBeRightPressed && !mappingConfig.triggers.rightPressed)
    {
        Keyboard.press(mappingConfig.triggers.keyRight);
        mappingConfig.triggers.rightPressed = true;
        Serial.print("RunAction: Right Trigger pressed -> Key ");
        Serial.println(mappingConfig.triggers.keyRight);
    }
    else if (!shouldBeRightPressed && mappingConfig.triggers.rightPressed)
    {
        Keyboard.release(mappingConfig.triggers.keyRight);
        mappingConfig.triggers.rightPressed = false;
        Serial.println("RunAction: Right Trigger released");
    }
}

void RunAction::processTriggerMouseX(int leftValue, int rightValue)
{
    unsigned long currentTime = millis();
    if (currentTime - lastTriggerUpdate < stickUpdateInterval)
    {
        return;
    }
    lastTriggerUpdate = currentTime;

    // Apply deadzone - triggers are 0-255, treat 0 as rest position
    int adjustedLeft = (leftValue > mappingConfig.triggers.deadzone) ? leftValue : 0;
    int adjustedRight = (rightValue > mappingConfig.triggers.deadzone) ? rightValue : 0;

    // Calculate net movement (right trigger moves right, left trigger moves left)
    int netMovement = adjustedRight - adjustedLeft;

    if (netMovement != 0)
    {
        int mouseX = (int)(netMovement * mappingConfig.triggers.sensitivity);

        Mouse.move(mouseX, 0, 0);
    }
}

void RunAction::processTriggerMouseY(int leftValue, int rightValue)
{
    unsigned long currentTime = millis();
    if (currentTime - lastTriggerUpdate < stickUpdateInterval)
    {
        return;
    }
    lastTriggerUpdate = currentTime;

    // Apply deadzone - triggers are 0-255, treat 0 as rest position
    int adjustedLeft = (leftValue > mappingConfig.triggers.deadzone) ? leftValue : 0;
    int adjustedRight = (rightValue > mappingConfig.triggers.deadzone) ? rightValue : 0;

    // Calculate net movement (right trigger moves down, left trigger moves up)
    int netMovement = adjustedRight - adjustedLeft;

    if (netMovement != 0)
    {
        int mouseY = (int)(netMovement * mappingConfig.triggers.sensitivity);

        Mouse.move(0, mouseY, 0);
    }
}

void RunAction::processTriggerScroll(int leftValue, int rightValue)
{
    unsigned long currentTime = millis();
    if (currentTime - lastTriggerUpdate < stickUpdateInterval)
    {
        return;
    }
    lastTriggerUpdate = currentTime;

    // Apply deadzone - triggers are 0-255, treatd 0 as rest position
    int adjustedLeft = (leftValue > mappingConfig.triggers.deadzone) ? leftValue : 0;
    int adjustedRight = (rightValue > mappingConfig.triggers.deadzone) ? rightValue : 0;

    // Calculate net movement (right trigger scrolls down, left trigger scrolls up)
    int netMovement = adjustedRight - adjustedLeft;

    if (netMovement != 0)
    {
        int scroll = (int)(netMovement * mappingConfig.triggers.sensitivity * 0.1f);
        if (scroll != 0)
        {
            Mouse.move(0, 0, -scroll); // Negative for natural scrolling
        }
    }
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
    mappingConfig.numMappings = 0;

    // Face buttons - WASdD
    mappingConfig.mappings[mappingConfig.numMappings++] = {GenericController::BTN_SOUTH, 's', false};
    mappingConfig.mappings[mappingConfig.numMappings++] = {GenericController::BTN_EAST, 'd', false};
    mappingConfig.mappings[mappingConfig.numMappings++] = {GenericController::BTN_WEST, 'a', false};
    mappingConfig.mappings[mappingConfig.numMappings++] = {GenericController::BTN_NORTH, 'w', false};

    // Shoulder buttons
    mappingConfig.mappings[mappingConfig.numMappings++] = {GenericController::BTN_L1, 'q', false};
    mappingConfig.mappings[mappingConfig.numMappings++] = {GenericController::BTN_R1, 'e', false};

    // Center buttons
    mappingConfig.mappings[mappingConfig.numMappings++] = {GenericController::BTN_START, 'r', false};
    mappingConfig.mappings[mappingConfig.numMappings++] = {GenericController::BTN_SELECT, 'f', false};

    // Stick clicks
    mappingConfig.mappings[mappingConfig.numMappings++] = {GenericController::BTN_L3, 't', false};
    mappingConfig.mappings[mappingConfig.numMappings++] = {GenericController::BTN_R3, 'g', false};

    // D-Pad - Arrow keys
    mappingConfig.mappings[mappingConfig.numMappings++] = {GenericController::BTN_DPAD_UP, KEY_UP, false};
    mappingConfig.mappings[mappingConfig.numMappings++] = {GenericController::BTN_DPAD_DOWN, KEY_DOWN, false};
    mappingConfig.mappings[mappingConfig.numMappings++] = {GenericController::BTN_DPAD_LEFT, KEY_LEFT, false};
    mappingConfig.mappings[mappingConfig.numMappings++] = {GenericController::BTN_DPAD_RIGHT, KEY_RIGHT, false};

    // Special
    mappingConfig.mappings[mappingConfig.numMappings++] = {GenericController::BTN_TOUCHPAD, 'h', false};

    Serial.print("RunAction: Initialized ");
    Serial.print(mappingConfig.numMappings);
    Serial.println(" default generic mappings");
}

void RunAction::initializeDefaultStickConfigs()
{
    mappingConfig.rightStick.behavior = StickBehavior::MOUSE_MOVEMENT;
    mappingConfig.leftStick.behavior = StickBehavior::ARROW_KEYS;
}

void RunAction::initializeDefaultTriggerConfigs()
{
    mappingConfig.triggers.behavior = TriggerBehavior::BUTTONS;
    mappingConfig.triggers.keyLeft = KEY_LEFT;
    mappingConfig.triggers.keyRight = KEY_RIGHT;
}

const char *RunAction::getGenericButtonName(uint8_t genericButton)
{
    return JoystickMapping::getGenericButtonName(genericButton);
}

void RunAction::setParams(RunActionParams p)
{
    params = p;
}

// Keyboard passthrough event handlers
void RunAction::onKeyPress(int unicode)
{
    // Pass through regular key presses
    Keyboard.press(unicode);
    Serial.print("RunAction: Keyboard passthrough: Key pressed - ");
    if (unicode >= 32 && unicode <= 126)
    {
        Serial.print((char)unicode);
        Serial.print(" ");
    }
    Serial.print("(0x");
    Serial.print(unicode, HEX);
    Serial.println(")");
}

void RunAction::onKeyRelease(int unicode)
{
    // Pass through regular key releases
    Keyboard.release(unicode);
    Serial.print("RunAction: Keyboard passthrough: Key released - ");
    if (unicode >= 32 && unicode <= 126)
    {
        Serial.print((char)unicode);
        Serial.print(" ");
    }
    Serial.print("(0x");
    Serial.print(unicode, HEX);
    Serial.println(")");
}

void RunAction::onExtrasPress(uint32_t top, uint16_t key)
{
    // Handle special keys (modifiers, function keys, etc.)
    Serial.print("RunAction: Keyboard passthrough: Extras pressed - top: 0x");
    Serial.print(top, HEX);
    Serial.print(", key: 0x");
    Serial.println(key, HEX);

    // Pass through the key using the key code format
    // The 'key' parameter contains the HID usage code
    Keyboard.press(key | 0xF000); // Use Teensy's extended keycode format
}

void RunAction::onExtrasRelease(uint32_t top, uint16_t key)
{
    // Handle special key releases
    Serial.print("RunAction: Keyboard passthrough: Extras released - top: 0x");
    Serial.print(top, HEX);
    Serial.print(", key: 0x");
    Serial.println(key, HEX);

    // Pass through the key release
    Keyboard.release(key | 0xF000);
}

void RunAction::DisplayLoadedFile()
{
    // Display loading message on LCD
    LiquidCrystal_I2C *lcd = devices->getLCD();
    lcd->clear();
    lcd->backlight();
    lcd->setCursor(3, 1);
    lcd->print("Running file:");

    int filenameLen = strlen(mappingConfig.displayName);
    int filenamePos = (20 - filenameLen) / 2; // Center on 20 character display
    lcd->setCursor(filenamePos, 2);
    lcd->print(mappingConfig.displayName);

    backlightOnTime = millis();
}