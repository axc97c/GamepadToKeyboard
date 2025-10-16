#include "actions/main_menu_action.h"
#include "actions/action_handler.h"
#include "devices.h"
#include "mapping/mapping_config.h"

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
        MenuItem("Save config", "save_config", 0),
        MenuItem("Edit config", "edit_config", 0)
    };

    setMenu(menuTitle, menuOptions, 3);

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
    else if (selectedItem.identifier == "save_config")
    {
        // Save the currently loaded config to its file
        Serial.print("Saving config to: ");
        Serial.println(mappingConfig.filename);

        LiquidCrystal_I2C *lcd = devices->getLCD();
        lcd->clear();
        lcd->setCursor(0, 0);
        lcd->print("Saving config...");

        bool success = MappingConfig::saveConfig(mappingConfig.filename, mappingConfig);

        lcd->clear();
        lcd->setCursor(0, 0);
        if (success)
        {
            Serial.println("Config saved successfully");
            lcd->print("Config saved!");
        }
        else
        {
            Serial.println("Failed to save config");
            lcd->print("Save failed!");
        }

        lcd->setCursor(0, 1);
        lcd->print(mappingConfig.filename);

        delay(2000);

        // Refresh the menu display
        refresh();
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