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
    String menuOptions[] = {
        "Load config",
        "Edit config"
    };
        
    setMenu(menuTitle, menuOptions, 2);

    Serial.println("MainMenuAction setup complete");
}

void MainMenuAction::onConfirm()
{
    // Handle confirmation for the main menu
    String selectedItem = getSelectedItem();
    int selectedIdx = getSelectedIndex();
    
    Serial.print("Main menu - Item confirmed: ");
    Serial.print(selectedItem);
    Serial.print(" (index: ");
    Serial.print(selectedIdx);
    Serial.println(")");
    
    // Handle each menu option
    if (selectedIdx == 0)
    {
        handler->activateLoadConfigMenu({0});
    }
    else if (selectedIdx == 1)
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