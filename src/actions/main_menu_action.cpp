#include "actions/main_menu_action.h"
#include "actions/action_handler.h"
#include "actions/text_input_action.h"
#include "devices.h"
#include "mapping/mapping_config.h"
#include "utils.h"

namespace {
    constexpr const char* MENU_LOAD_CONFIG = "load_config";
    constexpr const char* MENU_EDIT_CONFIG = "edit_config";
    constexpr const char* MENU_SAVE_CONFIG = "save_config";
    constexpr const char* MENU_SAVE_CONFIG_AS = "save_config_as";
}

MainMenuAction::MainMenuAction(DeviceManager *dev, ActionHandler *hdlr)
    : MenuAction(dev, hdlr)
{
    testInputBuffer[0] = '\0';

    clear();
    setTitle("Main Menu");
    addItem("Load config", MENU_LOAD_CONFIG);
    addItem("Edit config", MENU_EDIT_CONFIG);
    addItem("Save config", MENU_SAVE_CONFIG);
    addItem("Save config as...", MENU_SAVE_CONFIG_AS);
}

void MainMenuAction::onConfirm()
{
    // Handle confirmation for the main menu
    MenuItem selectedItem = getSelectedItem();

    Serial.print("MainMenuAction: Item confirmed: ");
    Serial.print(selectedItem.name);
    Serial.print(" (identifier: ");
    Serial.print(selectedItem.identifier);
    Serial.print(", data: ");
    Serial.print(selectedItem.data);
    Serial.println(")");

    // Handle each menu option using identifier (now char array - use strcmp)
    if (strcmp(selectedItem.identifier, MENU_LOAD_CONFIG) == 0)
    {
        handler->activateLoadConfigMenu();
    }
    else if (strcmp(selectedItem.identifier, MENU_SAVE_CONFIG) == 0)
    {
        performSave();
    }
    else if (strcmp(selectedItem.identifier, MENU_SAVE_CONFIG_AS) == 0)
    {
        // Open save as action
        handler->activateSaveAs();
    }
    else if (strcmp(selectedItem.identifier, MENU_EDIT_CONFIG) == 0)
    {
        handler->activateEditConfigMenu();
    }
}

void MainMenuAction::performSave()
{
    // Save the currently loaded config to its file
    Serial.print("MainMenuAction: Saving config to: ");
    Serial.println(mappingConfig.displayName);

    LiquidCrystal_I2C *lcd = devices->getLCD();
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print("Saving config...");

    bool success = MappingConfig::saveConfig(mappingConfig.filename, mappingConfig);

    lcd->clear();
    lcd->setCursor(0, 0);
    if (success)
    {
        Serial.println("MainMenuAction: Config saved successfully");
        lcd->print("Saved: ");
        lcd->print(mappingConfig.displayName);
    }
    else
    {
        Serial.println("MainMenuAction: Failed to save config");
        lcd->print("Save failed!");
        lcd->setCursor(0, 1);
        lcd->print(mappingConfig.displayName);
    }

    delay(2000);

    // Refresh the menu display
    refresh();
}
