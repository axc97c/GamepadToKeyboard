#include "actions/stick_mode_menu_action.h"
#include "actions/stick_config_menu_action.h"
#include "actions/action_handler.h"
#include "actions/text_input_action.h"
#include "devices.h"
#include "mapping/mapping_config.h"
#include "utils.h"

StickModeMenuAction::StickModeMenuAction(DeviceManager *dev, ActionHandler *hdlr, StickConfigActionParams p)
    : MenuAction(dev, hdlr), needsRefresh(false)
{
    stickParams = p;
}

void StickModeMenuAction::setStickParams(StickConfigActionParams p)
{
    stickParams = p;

    clear();

    addItem("Disabled", "disabled");
    addItem("Mouse", "mouse");
    addItem("Custom Keys", "keys");
    addItem("Scroll", "scroll");
    addItem("WASD Keys", "wasd");
    addItem("Arrow Keys", "arrows");
}

void StickModeMenuAction::loop()
{
    if (needsRefresh)
    {
        buildMenuItems();
        refresh();
        needsRefresh = false;
    }

    MenuAction::loop();
}

void StickModeMenuAction::onInit()
{
    stickConfig = stickParams.isRight ? &mappingConfig.rightStick : &mappingConfig.leftStick;
    buildMenuItems();
}

void StickModeMenuAction::buildMenuItems()
{
    char nameBuffer[MenuItem::MAX_NAME_LEN];
    snprintf(nameBuffer, sizeof(nameBuffer), "Stick Mode: %s", stickParams.isRight ? "Right" : "Left");
    setTitle(nameBuffer);
}

void StickModeMenuAction::onConfirm()
{
    MenuItem selectedItem = getSelectedItem();

    if (strcmp(selectedItem.identifier, "disabled") == 0)
    {
        stickConfig->behavior = StickBehavior::DISABLED;
    }
    else if (strcmp(selectedItem.identifier, "mouse") == 0)
    {
        stickConfig->behavior = StickBehavior::MOUSE_MOVEMENT;
    }
    else if (strcmp(selectedItem.identifier, "keys") == 0)
    {
        stickConfig->behavior = StickBehavior::BUTTON_EMULATION;
    }
    else if (strcmp(selectedItem.identifier, "scroll") == 0)
    {
        stickConfig->behavior = StickBehavior::SCROLL_WHEEL;
    }
    else if (strcmp(selectedItem.identifier, "wasd") == 0)
    {
        stickConfig->behavior = StickBehavior::WASD_KEYS;
    }
    else if (strcmp(selectedItem.identifier, "arrows") == 0)
    {
        stickConfig->behavior = StickBehavior::ARROW_KEYS;
    }

    handler->popAction();
}
