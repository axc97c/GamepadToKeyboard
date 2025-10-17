#include "actions/stick_mode_menu_action.h"
#include "actions/stick_config_menu_action.h"
#include "actions/action_handler.h"
#include "actions/text_input_action.h"
#include "devices.h"
#include "mapping/mapping_config.h"
#include "utils.h"

namespace {
    constexpr const char* STICK_MODE_DISABLED = "disabled";
    constexpr const char* STICK_MODE_MOUSE = "mouse";
    constexpr const char* STICK_MODE_KEYS = "keys";
    constexpr const char* STICK_MODE_SCROLL = "scroll";
    constexpr const char* STICK_MODE_WASD = "wasd";
    constexpr const char* STICK_MODE_ARROWS = "arrows";
}

StickModeMenuAction::StickModeMenuAction(DeviceManager *dev, ActionHandler *hdlr, StickConfigActionParams p)
    : MenuAction(dev, hdlr), needsRefresh(false)
{
    stickParams = p;
}

void StickModeMenuAction::setStickParams(StickConfigActionParams p)
{
    stickParams = p;

    clear();

    addItem("Disabled", STICK_MODE_DISABLED);
    addItem("Mouse", STICK_MODE_MOUSE);
    addItem("Custom Keys", STICK_MODE_KEYS);
    addItem("Scroll", STICK_MODE_SCROLL);
    addItem("WASD Keys", STICK_MODE_WASD);
    addItem("Arrow Keys", STICK_MODE_ARROWS);
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

    if (strcmp(selectedItem.identifier, STICK_MODE_DISABLED) == 0)
    {
        stickConfig->behavior = StickBehavior::DISABLED;
    }
    else if (strcmp(selectedItem.identifier, STICK_MODE_MOUSE) == 0)
    {
        stickConfig->behavior = StickBehavior::MOUSE_MOVEMENT;
    }
    else if (strcmp(selectedItem.identifier, STICK_MODE_KEYS) == 0)
    {
        stickConfig->behavior = StickBehavior::BUTTON_EMULATION;
    }
    else if (strcmp(selectedItem.identifier, STICK_MODE_SCROLL) == 0)
    {
        stickConfig->behavior = StickBehavior::SCROLL_WHEEL;
    }
    else if (strcmp(selectedItem.identifier, STICK_MODE_WASD) == 0)
    {
        stickConfig->behavior = StickBehavior::WASD_KEYS;
    }
    else if (strcmp(selectedItem.identifier, STICK_MODE_ARROWS) == 0)
    {
        stickConfig->behavior = StickBehavior::ARROW_KEYS;
    }

    handler->popAction();
}
