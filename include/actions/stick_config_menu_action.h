#ifndef STICK_CONFIG_MENU_ACTION_H
#define STICK_CONFIG_MENU_ACTION_H

#include "actions/menu_action.h"

class StickConfigMenuAction : public MenuAction
{
private:
    bool needsRefresh;

    void buildMenuItems();
    StickConfigActionParams stickParams;
    StickConfig* stickConfig;
    
public:
    StickConfigMenuAction(DeviceManager *dev, ActionHandler *hdlr, StickConfigActionParams p);

    void loop() override;
    void onInit() override;
    void onConfirm() override;

    void setStickParams(StickConfigActionParams p);
};

#endif // STICK_CONFIG_MENU_ACTION_H