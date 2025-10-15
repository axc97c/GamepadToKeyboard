#include "actions/edit_config_menu_action.h"
#include "actions/action_handler.h"
#include "mapping/mapping_config.h"
#include "mapping/joystick_mappings.h"
#include "mapping/keyboard_mapping.h"
#include "devices.h"
#include "utils.h"

EditConfigMenuAction::EditConfigMenuAction(DeviceManager *dev, ActionHandler *hdlr, MenuActionParams p)
    : MenuAction(dev, hdlr, p)
{
}

void EditConfigMenuAction::onInit()
{
    Serial.println("EditConfigMenuAction: Editing current config...");

    Serial.print("Editing config: ");
    Serial.println(mappingConfig.filename);
    Serial.print("Number of mappings: ");
    Serial.println(mappingConfig.numMappings);

    // Build the menu items directly from the current mappingConfig
    buildMenuItems();

    Serial.println("EditConfigMenuAction setup complete");
}

void EditConfigMenuAction::buildMenuItems()
{
    String displayName = Utils::trimFilename(mappingConfig.filename);

    menuTitle = "Edit " + displayName;

    // Create menu items - one per button mapping
    String items[JoystickMappingConfig::MAX_MAPPINGS];
    for (int i = 0; i < mappingConfig.numMappings; i++)
    {
        items[i] = getButtonKeyPair(i);

        Serial.print("Menu item ");
        Serial.print(i);
        Serial.print(": ");
        Serial.println(items[i]);
    }

    setMenu(menuTitle, items, mappingConfig.numMappings);
}

String EditConfigMenuAction::getButtonKeyPair(int index)
{
    if (index < 0 || index >= mappingConfig.numMappings)
    {
        return "Invalid";
    }

    // Get button name
    const char *buttonName = JoystickMapping::getGenericButtonName(mappingConfig.mappings[index].genericButton);

    // Get key name
    const char *keyName = KeyboardMapping::keyCodeToString(mappingConfig.mappings[index].keyCode);

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

    // TODO: Implement key remapping functionality
    // For now, just log which mapping was selected
    if (selectedIdx >= 0 && selectedIdx < mappingConfig.numMappings)
    {
        Serial.print("Selected mapping: ");
        Serial.print(JoystickMapping::getGenericButtonName(mappingConfig.mappings[selectedIdx].genericButton));
        Serial.print(" -> ");
        Serial.println(KeyboardMapping::keyCodeToString(mappingConfig.mappings[selectedIdx].keyCode));

        // Future: Open a key selection menu to change this mapping
    }
}

void EditConfigMenuAction::onCancel()
{
    // Handle cancel - return to previous action
    Serial.println("Edit config menu - Cancel/Back pressed, returning to previous action");

    handler->popAction();
}