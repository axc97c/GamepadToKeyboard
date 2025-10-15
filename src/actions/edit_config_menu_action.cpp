#include "actions/edit_config_menu_action.h"
#include "actions/action_handler.h"
#include "mapping/mapping_config.h"
#include "mapping/joystick_mappings.h"
#include "mapping/keyboard_mapping.h"
#include "devices.h"
#include "utils.h"

EditConfigMenuAction::EditConfigMenuAction(DeviceManager *dev, ActionHandler *hdlr, MenuActionParams p)
    : MenuAction(dev, hdlr, p), config()
{
}

void EditConfigMenuAction::onInit()
{
    Serial.println("EditConfigMenuAction: Loading current config...");
    
    // Get the current config filename from the handler
    const char *filename = handler->getLastRunFilename();
    strncpy(configFilename, filename, sizeof(configFilename) - 1);
    configFilename[sizeof(configFilename) - 1] = '\0';
    
    Serial.print("Editing config file: ");
    Serial.println(configFilename);
    
    // Load the current config
    loadCurrentConfig();
    
    Serial.println("EditConfigMenuAction setup complete");
}

void EditConfigMenuAction::loadCurrentConfig()
{
    // Load mappings from the config file
    if (!MappingConfig::loadConfig(configFilename, config))
    {
        Serial.println("Failed to load config file for editing");
        String errorItem[] = {"Error loading config"};
        menuTitle = "Edit Config";
        setMenu(menuTitle, errorItem, 1);
        return;
    }

    Serial.print("Loaded ");
    Serial.print(config.numMappings);
    Serial.println(" mappings for editing");

    // Build the menu items
    buildMenuItems();
}

void EditConfigMenuAction::buildMenuItems()
{
    String displayName = Utils::trimFilename(configFilename);

    menuTitle = "Edit " + displayName;

    // Create menu items - one per button mapping
    String items[JoystickMappingConfig::MAX_MAPPINGS];
    for (int i = 0; i < config.numMappings; i++)
    {
        items[i] = getButtonKeyPair(i);

        Serial.print("Menu item ");
        Serial.print(i);
        Serial.print(": ");
        Serial.println(items[i]);
    }

    setMenu(menuTitle, items, config.numMappings);
}

String EditConfigMenuAction::getButtonKeyPair(int index)
{
    if (index < 0 || index >= config.numMappings)
    {
        return "Invalid";
    }

    // Get button name
    const char *buttonName = JoystickMapping::getGenericButtonName(config.mappings[index].genericButton);

    // Get key name
    const char *keyName = KeyboardMapping::keyCodeToString(config.mappings[index].keyCode);

    // Format: "BTN_SOUTH -> s"
    // We need to shorten button names to fit on 20 char display
    String shortButtonName = String(buttonName);

    // Remove "BTN_" prefix if present
    if (shortButtonName.startsWith("BTN_"))
    {
        shortButtonName = shortButtonName.substring(4);
    }

    // Create the display string
    String result = shortButtonName + " > " + String(keyName);

    return result;
}

void EditConfigMenuAction::onConfirm()
{
    int selectedIdx = getSelectedIndex();
    String selectedItem = getSelectedItem();
    
    Serial.print("Edit config - Item confirmed: ");
    Serial.print(selectedItem);
    Serial.print(" (index: ");
    Serial.print(selectedIdx);
    Serial.println(")");
    
    // Check if there's an error state
    if (selectedItem == "Error loading config")
    {
        Serial.println("Cannot edit - error state");
        return;
    }
    
    // TODO: Implement key remapping functionality
    // For now, just log which mapping was selected
    if (selectedIdx >= 0 && selectedIdx < config.numMappings)
    {
        Serial.print("Selected mapping: ");
        Serial.print(JoystickMapping::getGenericButtonName(config.mappings[selectedIdx].genericButton));
        Serial.print(" -> ");
        Serial.println(KeyboardMapping::keyCodeToString(config.mappings[selectedIdx].keyCode));

        // Future: Open a key selection menu to change this mapping
    }
}

void EditConfigMenuAction::onCancel()
{
    // Handle cancel - return to main menu
    Serial.println("Edit config menu - Cancel/Back pressed, returning to main menu");
    
    handler->activateMainMenu({0});
}