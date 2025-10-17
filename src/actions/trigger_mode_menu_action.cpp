#include "actions/trigger_mode_menu_action.h"
#include "actions/action_handler.h"
#include "actions/text_input_action.h"
#include "devices.h"
#include "mapping/mapping_config.h"
#include "utils.h"

namespace {
    constexpr const char* TRIGGER_MODE_DISABLED = "disabled";
    constexpr const char* TRIGGER_MODE_MOUSE_X = "mouse_x";
    constexpr const char* TRIGGER_MODE_MOUSE_Y = "mouse_y";
    constexpr const char* TRIGGER_MODE_SCROLL = "scroll";
    constexpr const char* TRIGGER_MODE_KEYS = "keys";
    constexpr const char* TRIGGER_MODE_JOYSTICK_X = "joystick_x";
    constexpr const char* TRIGGER_MODE_JOYSTICK_Y = "joystick_y";
}

TriggerModeMenuAction::TriggerModeMenuAction(DeviceManager *dev, ActionHandler *hdlr)
    : MenuAction(dev, hdlr), needsRefresh(false)
{
    clear();

    setTitle("Trigger Mode");

    addItem("Disabled", TRIGGER_MODE_DISABLED);
    addItem("Mouse X", TRIGGER_MODE_MOUSE_X);
    addItem("Mouse Y", TRIGGER_MODE_MOUSE_Y);
    addItem("Scroll", TRIGGER_MODE_SCROLL);
    addItem("Keys", TRIGGER_MODE_KEYS);
    addItem("Joystick X", TRIGGER_MODE_JOYSTICK_X);
    addItem("Joystick Y", TRIGGER_MODE_JOYSTICK_Y);
}

void TriggerModeMenuAction::onConfirm()
{
    MenuItem selectedItem = getSelectedItem();

    if (strcmp(selectedItem.identifier, TRIGGER_MODE_DISABLED) == 0)
    {
        mappingConfig.triggers.behavior = TriggerBehavior::DISABLED;
    }
    else if (strcmp(selectedItem.identifier, TRIGGER_MODE_MOUSE_X) == 0)
    {
        mappingConfig.triggers.behavior = TriggerBehavior::MOUSE_X;
    }
    else if (strcmp(selectedItem.identifier, TRIGGER_MODE_MOUSE_Y) == 0)
    {
        mappingConfig.triggers.behavior = TriggerBehavior::MOUSE_Y;
    }
    else if (strcmp(selectedItem.identifier, TRIGGER_MODE_SCROLL) == 0)
    {
        mappingConfig.triggers.behavior = TriggerBehavior::SCROLL_WHEEL;
    }
    else if (strcmp(selectedItem.identifier, TRIGGER_MODE_KEYS) == 0)
    {
        mappingConfig.triggers.behavior = TriggerBehavior::BUTTONS;
    }
    else if (strcmp(selectedItem.identifier, TRIGGER_MODE_JOYSTICK_X) == 0)
    {
        mappingConfig.triggers.behavior = TriggerBehavior::JOYSTICK_X;
    }
    else if (strcmp(selectedItem.identifier, TRIGGER_MODE_JOYSTICK_Y) == 0)
    {
        mappingConfig.triggers.behavior = TriggerBehavior::JOYSTICK_Y;
    }

    handler->popAction();
}
