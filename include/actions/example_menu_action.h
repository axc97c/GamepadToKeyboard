#ifndef EXAMPLE_MENU_ACTION_H
#define EXAMPLE_MENU_ACTION_H

#include "actions/menu_action.h"

class ExampleMenuAction : public MenuAction
{
public:
    ExampleMenuAction(DeviceManager *dev, ActionHandler *hdlr, MenuActionParams p);

    // Implement pure virtual methods from MenuAction
    void onInit() override;
    void onConfirm() override;
    void onCancel() override;
};

#endif // EXAMPLE_MENU_ACTION_H