#ifndef EDIT_CONFIG_MENU_ACTION_H
#define EDIT_CONFIG_MENU_ACTION_H

#include "actions/menu_action.h"

class EditConfigMenuAction : public MenuAction
{
private:
    bool needsRefresh;

    void buildMenuItems();
    String getButtonKeyPair(int index);

public:
    EditConfigMenuAction(DeviceManager *dev, ActionHandler *hdlr, MenuActionParams p);

    // Override loop to handle refresh after returning from bind key action
    void loop() override;

    // Implement pure virtual methods from MenuAction
    void onInit() override;
    void onConfirm() override;
    void onCancel() override;
};

#endif // EDIT_CONFIG_MENU_ACTION_H