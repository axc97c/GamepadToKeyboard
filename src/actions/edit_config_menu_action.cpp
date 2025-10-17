#include "actions/edit_config_menu_action.h"
#include "actions/action_handler.h"
#include "mapping/mapping_config.h"
#include "mapping/joystick_mappings.h"
#include "mapping/keyboard_mapping.h"
#include "devices.h"
#include "utils.h"

EditConfigMenuAction::EditConfigMenuAction(DeviceManager *dev, ActionHandler *hdlr)
    : MenuAction(dev, hdlr), needsRefresh(false)
{
}

void EditConfigMenuAction::loop()
{
    if (needsRefresh)
    {
        buildMenuItems();
        refresh();
        needsRefresh = false;
    }

    MenuAction::loop();
}

void EditConfigMenuAction::onInit()
{
    buildMenuItems();
}

void EditConfigMenuAction::buildMenuItems()
{
    char nameBuffer[MenuItem::MAX_NAME_LEN];

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

    int itemCount = min(mappingConfig.numMappings, MAX_ITEMS);

    for (int i = 0; i < itemCount; i++)
    {
        const char *buttonName = JoystickMapping::getGenericButtonName(mappingConfig.mappings[i].genericButton);
        const char *keyName = KeyboardMapping::keyCodeToString(mappingConfig.mappings[i].keyCode);

        snprintf(nameBuffer, sizeof(nameBuffer), "%s > %s", buttonName, keyName);

        char idBuffer[MenuItem::MAX_ID_LEN];
        snprintf(idBuffer, sizeof(idBuffer), "mapping_%d", i);

        addItem(nameBuffer, idBuffer, i);
    }

    // Validate selectedIndex
    if (selectedIndex >= menuItemCount)
    {
        selectedIndex = menuItemCount > 0 ? menuItemCount - 1 : 0;
    }
}

String EditConfigMenuAction::getButtonKeyPair(int index)
{
    if (index < 0 || index >= mappingConfig.numMappings)
    {
        return String("Invalid");
    }

    const char *buttonName = JoystickMapping::getGenericButtonName(mappingConfig.mappings[index].genericButton);
    const char *keyName = KeyboardMapping::keyCodeToString(mappingConfig.mappings[index].keyCode);

    static char resultBuffer[32];

    snprintf(resultBuffer, sizeof(resultBuffer), "%s > %s", buttonName, keyName);

    return String(resultBuffer);
}

void EditConfigMenuAction::onConfirm()
{
    MenuItem selectedItem = getSelectedItem();

    // Use the data field which contains the mapping index
    int mappingIndex = selectedItem.data;
    needsRefresh = true;

    if (strcmp(selectedItem.identifier, "left_stick") == 0)
    {
        handler->activateStickConfigMenu({false});
        
        return;
    }
    else if (strcmp(selectedItem.identifier, "right_stick") == 0)
    {
        handler->activateStickConfigMenu({true});
        return;
    }

    // Open bind key action for the selected mapping
    if (mappingIndex >= 0 && mappingIndex < mappingConfig.numMappings)
    {
        BindKeyActionParams params;
        params.mappingIndex = mappingIndex;
        handler->activateBindKey(params);
        return;
    }
}
