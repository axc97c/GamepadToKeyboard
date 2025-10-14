#ifndef LOAD_CONFIG_MENU_ACTION_H
#define LOAD_CONFIG_MENU_ACTION_H

#include "actions/menu_action.h"

class LoadConfigMenuAction : public MenuAction
{
private:
    static const int MAX_CONFIG_FILES = 20;
    String configFiles[MAX_CONFIG_FILES]; // Store full filenames with path
    
    void scanConfigFiles();
    void sortConfigFiles(int count);

public:
    LoadConfigMenuAction(DeviceManager *dev, ActionHandler *hdlr, MenuActionParams p);

    // Implement pure virtual methods from MenuAction
    void onInit() override;
    void onConfirm() override;
    void onCancel() override;
};

#endif // LOAD_CONFIG_MENU_ACTION_H