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
        Serial.println("EditConfigMenuAction: Refreshing display after key binding");
        buildMenuItems();

        Serial.println("EditConfigMenuAction: Calling refresh()");
        refresh();

        needsRefresh = false;
        Serial.println("EditConfigMenuAction: Refresh complete");
    }

    // Call the base class loop to handle normal menu operations
    MenuAction::loop();
}

void EditConfigMenuAction::onInit()
{
    Serial.println("EditConfigMenuAction: Editing current config...");

    Serial.print("EditConfigMenuAction: Editing config: ");
    Serial.println(mappingConfig.displayName);
    Serial.print("EditConfigMenuAction: Number of mappings: ");
    Serial.println(mappingConfig.numMappings);

    // Build the menu items directly from the current mappingConfig
    buildMenuItems();

    Serial.println("EditConfigMenuAction: Setup complete");
}

void EditConfigMenuAction::buildMenuItems()
{
    Serial.println("EditConfigMenuAction: buildMenuItems START");
    char nameBuffer[MenuItem::MAX_NAME_LEN];

    // Clear all menu items using the clear() method
    clear();

    snprintf(nameBuffer, sizeof(nameBuffer), "Edit: %s", mappingConfig.displayName);
    setTitle(nameBuffer);

    const char *leftBehaviourName = MappingConfig::stickBehaviorToString(mappingConfig.leftStick.behavior);
    snprintf(nameBuffer, sizeof(nameBuffer), "Left > %s", leftBehaviourName);
    addItem(nameBuffer, "left_stick", 1);
    const char *rightBehaviourName = MappingConfig::stickBehaviorToString(mappingConfig.rightStick.behavior);
    snprintf(nameBuffer, sizeof(nameBuffer), "Right > %s", rightBehaviourName);
    addItem(nameBuffer, "right_stick", 1);
    addItem("Trigger", "triggers", 0);

    // Title already set in constructor
    int itemCount = min(mappingConfig.numMappings, MAX_ITEMS);

    // Build items directly into inherited menuItems array
    for (int i = 0; i < itemCount; i++)
    {
        // Get names as const char* to avoid allocations
        const char *buttonName = JoystickMapping::getGenericButtonName(mappingConfig.mappings[i].genericButton);
        const char *keyName = KeyboardMapping::keyCodeToString(mappingConfig.mappings[i].keyCode);

        // Skip "BTN_" prefix
        const char *displayButtonName = buttonName;
        if (strncmp(buttonName, "BTN_", 4) == 0)
        {
            displayButtonName = buttonName + 4;
        }

        // Build the display string directly into MenuItem char array - NO heap allocation!
        snprintf(nameBuffer, sizeof(nameBuffer), "%s > %s", displayButtonName, keyName);

        // Build identifier directly into MenuItem char array
        char idBuffer[MenuItem::MAX_ID_LEN];
        snprintf(idBuffer, sizeof(idBuffer), "mapping_%d", i);

        // Populate pre-allocated menuItems array using set() method
        addItem(nameBuffer, idBuffer, i);
    }

    // Validate selectedIndex
    if (selectedIndex >= menuItemCount)
    {
        selectedIndex = menuItemCount > 0 ? menuItemCount - 1 : 0;
    }

    Serial.println("EditConfigMenuAction: buildMenuItems DONE");
}

String EditConfigMenuAction::getButtonKeyPair(int index)
{
    Serial.print("EditConfigMenuAction: getButtonKeyPair(");
    Serial.print(index);
    Serial.print(")");

    if (index < 0 || index >= mappingConfig.numMappings)
    {
        Serial.println(" - Index out of range!");
        return String("Invalid");
    }

    Serial.println(" - OK");

    // Get button name (returns const char*, no allocation)
    const char *buttonName = JoystickMapping::getGenericButtonName(mappingConfig.mappings[index].genericButton);

    // Get key name (returns const char*, no allocation)
    const char *keyName = KeyboardMapping::keyCodeToString(mappingConfig.mappings[index].keyCode);

    // Build result string efficiently using a char buffer
    static char resultBuffer[32];
    const char *displayButtonName = buttonName;

    // Skip "BTN_" prefix if present
    if (strncmp(buttonName, "BTN_", 4) == 0)
    {
        displayButtonName = buttonName + 4;
    }

    // Format directly into buffer to avoid String concatenation
    snprintf(resultBuffer, sizeof(resultBuffer), "%s > %s", displayButtonName, keyName);

    Serial.print("EditConfigMenuAction: Result: ");
    Serial.println(resultBuffer);

    return String(resultBuffer);
}

void EditConfigMenuAction::onConfirm()
{
    MenuItem selectedItem = getSelectedItem();

    Serial.print("EditConfigMenuAction: Edit config - Item confirmed: ");
    Serial.print(selectedItem.name);
    Serial.print(" (identifier: ");
    Serial.print(selectedItem.identifier);
    Serial.print(", data: ");
    Serial.print(selectedItem.data);
    Serial.println(")");

    // Use the data field which contains the mapping index
    int mappingIndex = selectedItem.data;
    needsRefresh = true;

    if (strcmp(selectedItem.identifier, "left_stick") == 0)
    {
        handler->activateStickConfigMenu({0, false});
        
        return;
    }
    else if (strcmp(selectedItem.identifier, "right_stick") == 0)
    {
        handler->activateStickConfigMenu({0, true});
        return;
    }

    // Open bind key action for the selected mapping
    if (mappingIndex >= 0 && mappingIndex < mappingConfig.numMappings)
    {
        Serial.print("EditConfigMenuAction: Selected mapping: ");
        Serial.print(JoystickMapping::getGenericButtonName(mappingConfig.mappings[mappingIndex].genericButton));
        Serial.print(" -> ");
        Serial.println(KeyboardMapping::keyCodeToString(mappingConfig.mappings[mappingIndex].keyCode));

        // Open bind key action to change this mapping
        BindKeyActionParams params;
        params.mappingIndex = mappingIndex;
        handler->activateBindKey(params);
        return;
    }
}

void EditConfigMenuAction::onCancel()
{
    // Handle cancel - return to previous action
    Serial.println("EditConfigMenuAction: Edit config menu - Cancel/Back pressed, returning to previous action");

    handler->popAction();
}