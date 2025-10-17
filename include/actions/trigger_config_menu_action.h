#ifndef TRIGGER_CONFIG_MENU_ACTION_H
#define TRIGGER_CONFIG_MENU_ACTION_H

#include "actions/menu_action.h"

class TriggerConfigMenuAction : public MenuAction
{
private:
    bool needsRefresh;

    void buildMenuItems();
    
public:
    TriggerConfigMenuAction(DeviceManager *dev, ActionHandler *hdlr);

    void loop() override;
    void onInit() override;
    void onConfirm() override;
    void onLeft() override;
    void onRight() override;

    void changeValue(bool isDecrease);
};

#endif // TRIGGER_CONFIG_MENU_ACTION_H