#ifndef MAIN_MENU_ACTION_H
#define MAIN_MENU_ACTION_H

#include "actions/menu_action.h"

class MainMenuAction : public MenuAction
{
private:
    static const int TEST_INPUT_MAX_LENGTH = 15;
    char testInputBuffer[TEST_INPUT_MAX_LENGTH + 1];

    void performSave();
public:
    MainMenuAction(DeviceManager *dev, ActionHandler *hdlr, MenuActionParams p);

    // Implement pure virtual methods from MenuAction
    void onConfirm() override;
};

#endif // MAIN_MENU_ACTION_H