#ifndef STICK_MODE_MENU_ACTION_H
#define STICK_MODE_MENU_ACTION_H

#include "actions/menu_action.h"

class StickModeMenuAction : public MenuAction
{
private:
    bool needsRefresh;

    void buildMenuItems();
    StickConfigActionParams stickParams;
    StickConfig* stickConfig;
    
public:
    StickModeMenuAction(DeviceManager *dev, ActionHandler *hdlr, StickConfigActionParams p);

    void loop() override;
    void onInit() override;
    void onConfirm() override;

    void setStickParams(StickConfigActionParams p);
};

#endif // STICK_MODE_MENU_ACTION_H