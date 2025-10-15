#ifndef EDIT_CONFIG_MENU_ACTION_H
#define EDIT_CONFIG_MENU_ACTION_H

#include "actions/menu_action.h"

class EditConfigMenuAction : public MenuAction
{
private:
    void loadCurrentConfig();
    void buildMenuItems();
    String getButtonKeyPair(int index);

public:
    EditConfigMenuAction(DeviceManager *dev, ActionHandler *hdlr, MenuActionParams p);

    // Implement pure virtual methods from MenuAction
    void onInit() override;
    void onConfirm() override;
    void onCancel() override;
};

#endif // EDIT_CONFIG_MENU_ACTION_H