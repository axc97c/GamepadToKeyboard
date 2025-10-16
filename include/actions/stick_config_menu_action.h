#ifndef STICK_CONFIG_MENU_ACTION_H
#define STICK_CONFIG_MENU_ACTION_H

#include "actions/menu_action.h"

class StickConfigMenuAction : public MenuAction
{
private:
    bool needsRefresh;

    void buildMenuItems();
    String getButtonKeyPair(int index);

public:
    StickConfigMenuAction(DeviceManager *dev, ActionHandler *hdlr, StickConfigActionParams p);

    // Override loop to handle refresh after returning from bind key action
    void loop() override;

    // Implement pure virtual methods from MenuAction
    void onInit() override;
    void onConfirm() override;
    void onCancel() override;
};

#endif // STICK_CONFIG_MENU_ACTION_H