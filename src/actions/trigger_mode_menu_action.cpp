#include "actions/trigger_mode_menu_action.h"
#include "actions/action_handler.h"
#include "actions/text_input_action.h"
#include "devices.h"
#include "mapping/mapping_config.h"
#include "utils.h"

TriggerModeMenuAction::TriggerModeMenuAction(DeviceManager *dev, ActionHandler *hdlr)
    : MenuAction(dev, hdlr), needsRefresh(false)
{
    clear();

    setTitle("Trigger Mode");

    addItem("Disabled", "disabled");
    addItem("Mouse X", "mouse_x");
    addItem("Mouse Y", "mouse_y");
    addItem("Scroll", "scroll");
    addItem("Keys", "keys");
    addItem("Joystick X", "joystick_x");
    addItem("Joystick Y", "joystick_y");
}

void TriggerModeMenuAction::onConfirm()
{
    MenuItem selectedItem = getSelectedItem();

    if (strcmp(selectedItem.identifier, "disabled") == 0)
    {
        mappingConfig.triggers.behavior = TriggerBehavior::DISABLED;
    }
    else if (strcmp(selectedItem.identifier, "mouse_x") == 0)
    {
        mappingConfig.triggers.behavior = TriggerBehavior::MOUSE_X;
    }
    else if (strcmp(selectedItem.identifier, "mouse_y") == 0)
    {
        mappingConfig.triggers.behavior = TriggerBehavior::MOUSE_Y;
    }
    else if (strcmp(selectedItem.identifier, "scroll") == 0)
    {
        mappingConfig.triggers.behavior = TriggerBehavior::SCROLL_WHEEL;
    }
    else if (strcmp(selectedItem.identifier, "keys") == 0)
    {
        mappingConfig.triggers.behavior = TriggerBehavior::BUTTONS;
    }
    else if (strcmp(selectedItem.identifier, "joystick_x") == 0)
    {
        mappingConfig.triggers.behavior = TriggerBehavior::JOYSTICK_X;
    }
    else if (strcmp(selectedItem.identifier, "joystick_y") == 0)
    {
        mappingConfig.triggers.behavior = TriggerBehavior::JOYSTICK_Y;
    }

    handler->popAction();
}
