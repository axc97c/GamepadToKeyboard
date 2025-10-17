#include "actions/stick_config_menu_action.h"
#include "actions/action_handler.h"
#include "actions/text_input_action.h"
#include "devices.h"
#include "mapping/mapping_config.h"
#include "utils.h"

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
    addItem(nameBuffer, "mode", 1);

    snprintf(nameBuffer, sizeof(nameBuffer), "Sensitivity: %.2f", stickConfig->sensitivity);
    addItem(nameBuffer, "sensitivity");

    snprintf(nameBuffer, sizeof(nameBuffer), "Deadzone: %d", stickConfig->deadzone);
    addItem(nameBuffer, "deadzone");

    snprintf(nameBuffer, sizeof(nameBuffer), "Threshold: %d", stickConfig->activationThreshold);
    addItem(nameBuffer, "threshold");

    addItem("Up > Up", "up");
    addItem("Down > Down", "down");
    addItem("Left > Left", "left");
    addItem("Right > Right", "right");
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
    if (strcmp(selectedItem.identifier, "mode") == 0)
    {
        handler->activateStickModeMenu(stickParams);
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

    if (strcmp(selectedItem.identifier, "sensitivity") == 0)
    {
        needsRefresh = true;
        stickConfig->sensitivity += isDecrease ? -0.01 : 0.01;
    }
    else if (strcmp(selectedItem.identifier, "deadzone") == 0)
    {
        needsRefresh = true;
        stickConfig->deadzone += isDecrease ? -1 : 1;
    }
    else if (strcmp(selectedItem.identifier, "threshold") == 0)
    {
        needsRefresh = true;
        stickConfig->activationThreshold += isDecrease ? -1 : 1;
    }
}