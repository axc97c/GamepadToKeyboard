#include "actions/main_menu_action.h"
#include "actions/action_handler.h"
#include "devices.h"

MainMenuAction::MainMenuAction(DeviceManager *dev, ActionHandler *hdlr, MenuActionParams p)
    : MenuAction(dev, hdlr, p)
{
}

void MainMenuAction::onInit()
{
    // Initialize the main menu
    menuTitle = "Main Menu";
    MenuItem menuOptions[] = {
        MenuItem("Load config", "load_config", 0),
        MenuItem("Edit config", "edit_config", 0)
    };

    setMenu(menuTitle, menuOptions, 2);

    Serial.println("MainMenuAction setup complete");
}

void MainMenuAction::onConfirm()
{
    // Handle confirmation for the main menu
    MenuItem selectedItem = getSelectedItem();
    int selectedIdx = getSelectedIndex();

    Serial.print("Main menu - Item confirmed: ");
    Serial.print(selectedItem.name);
    Serial.print(" (identifier: ");
    Serial.print(selectedItem.identifier);
    Serial.print(", data: ");
    Serial.print(selectedItem.data);
    Serial.println(")");

    // Handle each menu option using identifier
    if (selectedItem.identifier == "load_config")
    {
        handler->activateLoadConfigMenu({0});
    }
    else if (selectedItem.identifier == "edit_config")
    {
        handler->activateEditConfigMenu({0});
    }
}

void MainMenuAction::onCancel()
{
    // Handle cancel for the main menu - return to previous action (Run)
    Serial.println("Main menu - Cancel/Back pressed, returning to previous action");

    handler->popAction();
}