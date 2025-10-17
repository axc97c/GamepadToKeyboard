#include "actions/stick_config_menu_action.h"
#include "actions/action_handler.h"
#include "actions/text_input_action.h"
#include "devices.h"
#include "mapping/mapping_config.h"
#include "mapping/keyboard_mapping.h"
#include "utils.h"

namespace {
    constexpr const char* STICK_CONFIG_MODE = "mode";
    constexpr const char* STICK_CONFIG_SENSITIVITY = "sensitivity";
    constexpr const char* STICK_CONFIG_DEADZONE = "deadzone";
    constexpr const char* STICK_CONFIG_THRESHOLD = "threshold";
    constexpr const char* STICK_CONFIG_UP = "up";
    constexpr const char* STICK_CONFIG_DOWN = "down";
    constexpr const char* STICK_CONFIG_LEFT = "left";
    constexpr const char* STICK_CONFIG_RIGHT = "right";
}

StickConfigMenuAction::StickConfigMenuAction(DeviceManager *dev, ActionHandler *hdlr, StickConfigActionParams p)
    : MenuAction(dev, hdlr), needsRefresh(false)
{
    stickParams = p;
}

void StickConfigMenuAction::setStickParams(StickConfigActionParams p)
{
    stickParams = p;
}

void StickConfigMenuAction::loop()
{
    if (needsRefresh)
    {
        buildMenuItems();
        refresh();
        needsRefresh = false;
    }

    MenuAction::loop();
}

void StickConfigMenuAction::onInit()
{
    stickConfig = stickParams.isRight ? &mappingConfig.rightStick : &mappingConfig.leftStick;
    buildMenuItems();
}

void StickConfigMenuAction::buildMenuItems()
{
    clear();

    char nameBuffer[MenuItem::MAX_NAME_LEN];
    snprintf(nameBuffer, sizeof(nameBuffer), "Stick: %s", stickParams.isRight ? "Right" : "Left");
    setTitle(nameBuffer);

    const char *leftBehaviourName = MappingConfig::stickBehaviorToString(stickConfig->behavior);
    snprintf(nameBuffer, sizeof(nameBuffer), "Mode: %s", leftBehaviourName);
    addItem(nameBuffer, STICK_CONFIG_MODE, 1);

    snprintf(nameBuffer, sizeof(nameBuffer), "Sensitivity: %.2f", stickConfig->sensitivity);
    addItem(nameBuffer, STICK_CONFIG_SENSITIVITY);

    snprintf(nameBuffer, sizeof(nameBuffer), "Deadzone: %d", stickConfig->deadzone);
    addItem(nameBuffer, STICK_CONFIG_DEADZONE);

    snprintf(nameBuffer, sizeof(nameBuffer), "Threshold: %d", stickConfig->activationThreshold);
    addItem(nameBuffer, STICK_CONFIG_THRESHOLD);

    const char *upKeyName = KeyboardMapping::keyCodeToString(stickConfig->keyUp);
    snprintf(nameBuffer, sizeof(nameBuffer), "Up > %s", upKeyName);
    addItem(nameBuffer, STICK_CONFIG_UP);

    const char *downKeyName = KeyboardMapping::keyCodeToString(stickConfig->keyDown);
    snprintf(nameBuffer, sizeof(nameBuffer), "Down > %s", downKeyName);
    addItem(nameBuffer, STICK_CONFIG_DOWN);

    const char *leftKeyName = KeyboardMapping::keyCodeToString(stickConfig->keyLeft);
    snprintf(nameBuffer, sizeof(nameBuffer), "Left > %s", leftKeyName);
    addItem(nameBuffer, STICK_CONFIG_LEFT);

    const char *rightKeyName = KeyboardMapping::keyCodeToString(stickConfig->keyRight);
    snprintf(nameBuffer, sizeof(nameBuffer), "Right > %s", rightKeyName);
    addItem(nameBuffer, STICK_CONFIG_RIGHT);
}

void StickConfigMenuAction::onConfirm()
{
    MenuItem selectedItem = getSelectedItem();

    Serial.print("StickConfigMenuAction: Item confirmed: ");
    Serial.print(selectedItem.name);
    Serial.print(" (identifier: ");
    Serial.print(selectedItem.identifier);
    Serial.print(", data: ");
    Serial.print(selectedItem.data);
    Serial.println(")");

    // Handle each menu option using identifier (now char array - use strcmp)
    if (strcmp(selectedItem.identifier, STICK_CONFIG_MODE) == 0)
    {
        handler->activateStickModeMenu(stickParams);
    }
    else if (strcmp(selectedItem.identifier, STICK_CONFIG_UP) == 0)
    {
        BindKeyActionParams params;
        params.target = BindKeyTarget::STICK_UP;
        params.mappingIndex = -1;
        params.isRightStick = stickParams.isRight;
        needsRefresh = true;
        handler->activateBindKey(params);
    }
    else if (strcmp(selectedItem.identifier, STICK_CONFIG_DOWN) == 0)
    {
        BindKeyActionParams params;
        params.target = BindKeyTarget::STICK_DOWN;
        params.mappingIndex = -1;
        params.isRightStick = stickParams.isRight;
        needsRefresh = true;
        handler->activateBindKey(params);
    }
    else if (strcmp(selectedItem.identifier, STICK_CONFIG_LEFT) == 0)
    {
        BindKeyActionParams params;
        params.target = BindKeyTarget::STICK_LEFT;
        params.mappingIndex = -1;
        params.isRightStick = stickParams.isRight;
        needsRefresh = true;
        handler->activateBindKey(params);
    }
    else if (strcmp(selectedItem.identifier, STICK_CONFIG_RIGHT) == 0)
    {
        BindKeyActionParams params;
        params.target = BindKeyTarget::STICK_RIGHT;
        params.mappingIndex = -1;
        params.isRightStick = stickParams.isRight;
        needsRefresh = true;
        handler->activateBindKey(params);
    }
}

void StickConfigMenuAction::onLeft()
{
    changeValue(true);
}

void StickConfigMenuAction::onRight()
{
    changeValue(false);
}

void StickConfigMenuAction::changeValue(bool isDecrease)
{
    MenuItem selectedItem = getSelectedItem();

    if (strcmp(selectedItem.identifier, STICK_CONFIG_SENSITIVITY) == 0)
    {
        needsRefresh = true;
        stickConfig->sensitivity += isDecrease ? -0.01 : 0.01;
    }
    else if (strcmp(selectedItem.identifier, STICK_CONFIG_DEADZONE) == 0)
    {
        needsRefresh = true;
        stickConfig->deadzone += isDecrease ? -1 : 1;
    }
    else if (strcmp(selectedItem.identifier, STICK_CONFIG_THRESHOLD) == 0)
    {
        needsRefresh = true;
        stickConfig->activationThreshold += isDecrease ? -1 : 1;
    }
}