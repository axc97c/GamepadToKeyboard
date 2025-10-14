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
        // Load config selected
        Serial.println("Load config selected");
        handler->activateLoadConfigMenu({0});
    }
    else if (selectedIdx == 1)
    {
        // Edit config selected
        Serial.println("Edit config selected");
        // TODO: Switch to config editor
        // handler->activateConfigEdit({0});
    }
}

void MainMenuAction::onCancel()
{
    // Handle cancel for the main menu - return to run action
    Serial.println("Main menu - Cancel/Back pressed, returning to run action");
    
    // Get the last run action filename from handler
    const char *filename = handler->getLastRunFilename();
    
    handler->activateRun({filename});
}