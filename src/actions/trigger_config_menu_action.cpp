#include "actions/trigger_config_menu_action.h"
#include "actions/action_handler.h"
#include "actions/text_input_action.h"
#include "devices.h"
#include "mapping/mapping_config.h"
#include "mapping/keyboard_mapping.h"
#include "utils.h"

namespace {
    constexpr const char* TRIGGER_CONFIG_MODE = "mode";
    constexpr const char* TRIGGER_CONFIG_SENSITIVITY = "sensitivity";
    constexpr const char* TRIGGER_CONFIG_DEADZONE = "deadzone";
    constexpr const char* TRIGGER_CONFIG_THRESHOLD = "threshold";
    constexpr const char* TRIGGER_CONFIG_LEFT = "left";
    constexpr const char* TRIGGER_CONFIG_RIGHT = "right";
}

TriggerConfigMenuAction::TriggerConfigMenuAction(DeviceManager *dev, ActionHandler *hdlr)
    : MenuAction(dev, hdlr), needsRefresh(false)
{
    setTitle("Triggers:");
}

void TriggerConfigMenuAction::loop()
{
    if (needsRefresh)
    {
        buildMenuItems();
        refresh();
        needsRefresh = false;
    }

    MenuAction::loop();
}

void TriggerConfigMenuAction::onInit()
{
    buildMenuItems();
}

void TriggerConfigMenuAction::buildMenuItems()
{
    clear();

    char nameBuffer[MenuItem::MAX_NAME_LEN];

    const char *leftBehaviourName = MappingConfig::triggerBehaviorToString(mappingConfig.triggers.behavior);
    snprintf(nameBuffer, sizeof(nameBuffer), "Mode: %s", leftBehaviourName);
    addItem(nameBuffer, TRIGGER_CONFIG_MODE, 1);

    snprintf(nameBuffer, sizeof(nameBuffer), "Sensitivity: %.2f", mappingConfig.triggers.sensitivity);
    addItem(nameBuffer, TRIGGER_CONFIG_SENSITIVITY);

    snprintf(nameBuffer, sizeof(nameBuffer), "Deadzone: %d", mappingConfig.triggers.deadzone);
    addItem(nameBuffer, TRIGGER_CONFIG_DEADZONE);

    snprintf(nameBuffer, sizeof(nameBuffer), "Threshold: %d", mappingConfig.triggers.activationThreshold);
    addItem(nameBuffer, TRIGGER_CONFIG_THRESHOLD);

    const char *leftKeyName = KeyboardMapping::keyCodeToString(mappingConfig.triggers.keyLeft);
    snprintf(nameBuffer, sizeof(nameBuffer), "Left > %s", leftKeyName);
    addItem(nameBuffer, TRIGGER_CONFIG_LEFT);

    const char *rightKeyName = KeyboardMapping::keyCodeToString(mappingConfig.triggers.keyRight);
    snprintf(nameBuffer, sizeof(nameBuffer), "Right > %s", rightKeyName);
    addItem(nameBuffer, TRIGGER_CONFIG_RIGHT);
}

void TriggerConfigMenuAction::onConfirm()
{
    MenuItem selectedItem = getSelectedItem();

    if (strcmp(selectedItem.identifier, TRIGGER_CONFIG_MODE) == 0)
    {
        handler->activateTriggerModeMenu();
    }
    else if (strcmp(selectedItem.identifier, TRIGGER_CONFIG_LEFT) == 0)
    {
        BindKeyActionParams params;
        params.target = BindKeyTarget::TRIGGER_LEFT;
        params.mappingIndex = -1; // Not used for trigger keys
        params.isRightStick = false; // Not used for trigger keys
        needsRefresh = true;
        handler->activateBindKey(params);
    }
    else if (strcmp(selectedItem.identifier, TRIGGER_CONFIG_RIGHT) == 0)
    {
        BindKeyActionParams params;
        params.target = BindKeyTarget::TRIGGER_RIGHT;
        params.mappingIndex = -1; // Not used for trigger keys
        params.isRightStick = false; // Not used for trigger keys
        needsRefresh = true;
        handler->activateBindKey(params);
    }
}

void TriggerConfigMenuAction::onLeft()
{
    changeValue(true);
}

void TriggerConfigMenuAction::onRight()
{
    changeValue(false);
}

void TriggerConfigMenuAction::changeValue(bool isDecrease)
{
    MenuItem selectedItem = getSelectedItem();

    if (strcmp(selectedItem.identifier, TRIGGER_CONFIG_SENSITIVITY) == 0)
    {
        needsRefresh = true;
        mappingConfig.triggers.sensitivity += isDecrease ? -0.01 : 0.01;
    }
    else if (strcmp(selectedItem.identifier, TRIGGER_CONFIG_DEADZONE) == 0)
    {
        needsRefresh = true;
        mappingConfig.triggers.deadzone += isDecrease ? -1 : 1;
    }
    else if (strcmp(selectedItem.identifier, TRIGGER_CONFIG_THRESHOLD) == 0)
    {
        needsRefresh = true;
        mappingConfig.triggers.activationThreshold += isDecrease ? -1 : 1;
    }
}