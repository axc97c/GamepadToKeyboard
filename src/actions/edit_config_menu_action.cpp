#include "actions/edit_config_menu_action.h"
#include "actions/action_handler.h"
#include "mapping/mapping_config.h"
#include "mapping/joystick_mappings.h"
#include "mapping/keyboard_mapping.h"
#include "devices.h"
#include "utils.h"

EditConfigMenuAction::EditConfigMenuAction(DeviceManager *dev, ActionHandler *hdlr, MenuActionParams p)
    : MenuAction(dev, hdlr, p), needsRefresh(false)
{
}

void EditConfigMenuAction::loop()
{
    // Check if we need to refresh the menu (after returning from bind key action)
    if (needsRefresh)
    {
        buildMenuItems();
        refresh();
        needsRefresh = false;
        Serial.println("EditConfigMenuAction: Menu refreshed after key binding");
    }

    // Call the base class loop to handle normal menu operations
    MenuAction::loop();
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
    MenuItem items[JoystickMappingConfig::MAX_MAPPINGS];
    for (int i = 0; i < mappingConfig.numMappings; i++)
    {
        String buttonKeyPair = getButtonKeyPair(i);
        String identifier = String("mapping_") + String(i);
        items[i] = MenuItem(buttonKeyPair, identifier, i);

        Serial.print("Menu item ");
        Serial.print(i);
        Serial.print(": ");
        Serial.println(buttonKeyPair);
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
    MenuItem selectedItem = getSelectedItem();
    int selectedIdx = getSelectedIndex();

    Serial.print("Edit config - Item confirmed: ");
    Serial.print(selectedItem.name);
    Serial.print(" (identifier: ");
    Serial.print(selectedItem.identifier);
    Serial.print(", data: ");
    Serial.print(selectedItem.data);
    Serial.println(")");

    // Use the data field which contains the mapping index
    int mappingIndex = selectedItem.data;

    // Open bind key action for the selected mapping
    if (mappingIndex >= 0 && mappingIndex < mappingConfig.numMappings)
    {
        Serial.print("Selected mapping: ");
        Serial.print(JoystickMapping::getGenericButtonName(mappingConfig.mappings[mappingIndex].genericButton));
        Serial.print(" -> ");
        Serial.println(KeyboardMapping::keyCodeToString(mappingConfig.mappings[mappingIndex].keyCode));

        // Open bind key action to change this mapping
        BindKeyActionParams params;
        params.mappingIndex = mappingIndex;
        handler->activateBindKey(params);

        // Mark that we need to refresh when we return
        needsRefresh = true;
    }
}

void EditConfigMenuAction::onCancel()
{
    // Handle cancel - return to previous action
    Serial.println("Edit config menu - Cancel/Back pressed, returning to previous action");

    handler->popAction();
}