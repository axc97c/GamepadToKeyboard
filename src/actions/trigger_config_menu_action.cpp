#include "actions/trigger_config_menu_action.h"
#include "actions/action_handler.h"
#include "actions/text_input_action.h"
#include "devices.h"
#include "mapping/mapping_config.h"
#include "utils.h"

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
    addItem(nameBuffer, "mode", 1);

    snprintf(nameBuffer, sizeof(nameBuffer), "Sensitivity: %.2f", mappingConfig.triggers.sensitivity);
    addItem(nameBuffer, "sensitivity");

    snprintf(nameBuffer, sizeof(nameBuffer), "Deadzone: %d", mappingConfig.triggers.deadzone);
    addItem(nameBuffer, "deadzone");

    snprintf(nameBuffer, sizeof(nameBuffer), "Threshold: %d", mappingConfig.triggers.activationThreshold);
    addItem(nameBuffer, "threshold");

    addItem("Left > Left", "left");
    addItem("Right > Right", "right");
}

void TriggerConfigMenuAction::onConfirm()
{
    MenuItem selectedItem = getSelectedItem();

    if (strcmp(selectedItem.identifier, "mode") == 0)
    {
        handler->activateTriggerModeMenu();
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

    if (strcmp(selectedItem.identifier, "sensitivity") == 0)
    {
        needsRefresh = true;
        mappingConfig.triggers.sensitivity += isDecrease ? -0.01 : 0.01;
    }
    else if (strcmp(selectedItem.identifier, "deadzone") == 0)
    {
        needsRefresh = true;
        mappingConfig.triggers.deadzone += isDecrease ? -1 : 1;
    }
    else if (strcmp(selectedItem.identifier, "threshold") == 0)
    {
        needsRefresh = true;
        mappingConfig.triggers.activationThreshold += isDecrease ? -1 : 1;
    }
}