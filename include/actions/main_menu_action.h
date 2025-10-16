#ifndef MAIN_MENU_ACTION_H
#define MAIN_MENU_ACTION_H

#include "actions/menu_action.h"

class MainMenuAction : public MenuAction
{
private:
    void performSave();

public:
    MainMenuAction(DeviceManager *dev, ActionHandler *hdlr, MenuActionParams p);

    // Implement pure virtual methods from MenuAction
    void onInit() override;
    void onConfirm() override;
    void onCancel() override;
};

#endif // MAIN_MENU_ACTION_H