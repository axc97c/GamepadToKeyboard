#ifndef TRIGGER_MODE_MENU_ACTION_H
#define TRIGGER_MODE_MENU_ACTION_H

#include "actions/menu_action.h"

class TriggerModeMenuAction : public MenuAction
{
private:
    bool needsRefresh;

    void buildMenuItems();
    
public:
    TriggerModeMenuAction(DeviceManager *dev, ActionHandler *hdlr);

    void onConfirm() override;
};

#endif // TRIGGER_MODE_MENU_ACTION_H