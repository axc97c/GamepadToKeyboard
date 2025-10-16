#include "actions/stick_config_menu_action.h"
#include "actions/action_handler.h"
#include "actions/text_input_action.h"
#include "devices.h"
#include "mapping/mapping_config.h"
#include "utils.h"

StickConfigMenuAction::StickConfigMenuAction(DeviceManager *dev, ActionHandler *hdlr, StickConfigActionParams p)
    : MenuAction(dev, hdlr, p)
{
    // Set the fixed title in constructor since it never changes
    // setTitle("Main Menu");

    // // Populate fixed menu items in pre-allocated array (no dynamic allocation!)
    // menuItems[0].set("Load config", "load_config", 0);
    // menuItems[1].set("Edit config", "edit_config", 0);
    // menuItems[2].set("Save config", "save_config", 0);
    // menuItems[3].set("Save config as...", "save_config_as", 0);
    // menuItemCount = 4;
}

void StickConfigMenuAction::onInit()
{
    Serial.println("StickConfigMenuAction: Setting up");
    // Menu items already populated in constructor - nothing to do here!
    Serial.println("StickConfigMenuAction: Setup complete");
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

void StickConfigMenuAction::onCancel()
{
    // Handle cancel for the main menu - return to previous action (Run)
    Serial.println("StickConfigMenuAction: Cancel/Back pressed, returning to previous action");

    handler->popAction();
}
