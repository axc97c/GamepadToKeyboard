#include "actions/stick_config_menu_action.h"
#include "actions/action_handler.h"
#include "actions/text_input_action.h"
#include "devices.h"
#include "mapping/mapping_config.h"
#include "utils.h"

StickConfigMenuAction::StickConfigMenuAction(DeviceManager *dev, ActionHandler *hdlr, StickConfigActionParams p)
    : MenuAction(dev, hdlr, p), needsRefresh(false)
{
    stickParams = p;
}

void StickConfigMenuAction::setStickParams(StickConfigActionParams p)
{
    stickParams = p;
    MenuAction::setParams(p);
}

void StickConfigMenuAction::loop()
{
    // Check if we need to refresh the menu (after returning from bind key action)
    if (needsRefresh)
    {
        Serial.println("EditConfigMenuAction: Refreshing display after key binding");
        buildMenuItems();
        refresh();
        needsRefresh = false;
        Serial.println("EditConfigMenuAction: Refresh complete");
    }

    // Call the base class loop to handle normal menu operations
    MenuAction::loop();
}

void StickConfigMenuAction::onInit()
{

    Serial.println("StickConfigMenuAction: Setting up");
    stickConfig = stickParams.isRight ? &mappingConfig.rightStick : &mappingConfig.leftStick;
    buildMenuItems();
    Serial.println("StickConfigMenuAction: Setup complete");
}

void StickConfigMenuAction::buildMenuItems()
{
    clear();

    char nameBuffer[MenuItem::MAX_NAME_LEN];
    snprintf(nameBuffer, sizeof(nameBuffer), "Stick: %s", stickParams.isRight ? "Right" : "Left");
    setTitle(nameBuffer);

    const char *leftBehaviourName = MappingConfig::stickBehaviorToString(stickConfig->behavior);
    snprintf(nameBuffer, sizeof(nameBuffer), "Mode > %s", leftBehaviourName);
    addItem(nameBuffer, "mode", 1);
}

void StickConfigMenuAction::onConfirm()
{
    // Handle confirmation for the main menu
    MenuItem selectedItem = getSelectedItem();

    Serial.print("StickConfigMenuAction: Item confirmed: ");
    Serial.print(selectedItem.name);
    Serial.print(" (identifier: ");
    Serial.print(selectedItem.identifier);
    Serial.print(", data: ");
    Serial.print(selectedItem.data);
    Serial.println(")");

    // Handle each menu option using identifier (now char array - use strcmp)
    // if (strcmp(selectedItem.identifier, "load_config") == 0)
    // {
    //     handler->activateLoadConfigMenu({0});
    // }
    // else if (strcmp(selectedItem.identifier, "save_config") == 0)
    // {
    //     performSave();
    // }
    // else if (strcmp(selectedItem.identifier, "save_config_as") == 0)
    // {
    //     // Open save as action
    //     handler->activateSaveAs();
    // }
    // else if (strcmp(selectedItem.identifier, "edit_config") == 0)
    // {
    //     handler->activateEditConfigMenu({0});
    // }
}
