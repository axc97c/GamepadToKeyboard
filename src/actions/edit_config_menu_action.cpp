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

        // Just rebuild the ONE item that changed, not the whole menu!
        int idx = selectedIndex;
        if (idx >= 0 && idx < mappingConfig.numMappings && idx < menuItemCount)
        {
            Serial.print("Updating item ");
            Serial.println(idx);

            const char *buttonName = JoystickMapping::getGenericButtonName(mappingConfig.mappings[idx].genericButton);
            const char *keyName = KeyboardMapping::keyCodeToString(mappingConfig.mappings[idx].keyCode);

            const char *displayButtonName = buttonName;
            if (strncmp(buttonName, "BTN_", 4) == 0)
            {
                displayButtonName = buttonName + 4;
            }

            static char nameBuffer[32];
            snprintf(nameBuffer, sizeof(nameBuffer), "%s > %s", displayButtonName, keyName);

            menuItems[idx].name = String(nameBuffer);
            Serial.print("Updated to: ");
            Serial.println(menuItems[idx].name);
        }

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
    Serial.println("buildMenuItems START");

    // Clear existing menu items to free String memory
    for (int i = 0; i < menuItemCount; i++)
    {
        menuItems[i].name = "";
        menuItems[i].identifier = "";
    }
    menuItemCount = 0;

    Serial.println("Cleared old items");

    // Set title directly on inherited member to avoid String allocation
    menuTitle = "Edit Config";

    int itemCount = min(mappingConfig.numMappings, MAX_ITEMS);
    Serial.print("Items: ");
    Serial.println(itemCount);

    // Build items directly into inherited menuItems array
    for (int i = 0; i < itemCount; i++)
    {
        Serial.print("Item ");
        Serial.println(i);

        // Get names as const char* to avoid allocations
        const char *buttonName = JoystickMapping::getGenericButtonName(mappingConfig.mappings[i].genericButton);
        const char *keyName = KeyboardMapping::keyCodeToString(mappingConfig.mappings[i].keyCode);

        // Skip "BTN_" prefix
        const char *displayButtonName = buttonName;
        if (strncmp(buttonName, "BTN_", 4) == 0)
        {
            displayButtonName = buttonName + 4;
        }

        // Build the display string
        static char nameBuffer[32];
        snprintf(nameBuffer, sizeof(nameBuffer), "%s > %s", displayButtonName, keyName);

        // Build identifier
        static char idBuffer[16];
        snprintf(idBuffer, sizeof(idBuffer), "mapping_%d", i);

        // Assign to inherited menuItems array
        menuItems[i].name = String(nameBuffer);
        menuItems[i].identifier = String(idBuffer);
        menuItems[i].data = i;
    }

    // Set the count
    menuItemCount = itemCount;

    // Validate selectedIndex
    if (selectedIndex >= menuItemCount)
    {
        selectedIndex = menuItemCount > 0 ? menuItemCount - 1 : 0;
    }

    Serial.println("buildMenuItems DONE");
}

String EditConfigMenuAction::getButtonKeyPair(int index)
{
    Serial.print("getButtonKeyPair(");
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

    Serial.print("Result: ");
    Serial.println(resultBuffer);

    return String(resultBuffer);
}

void EditConfigMenuAction::onConfirm()
{
    MenuItem selectedItem = getSelectedItem();

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