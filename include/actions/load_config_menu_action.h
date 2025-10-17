#ifndef LOAD_CONFIG_MENU_ACTION_H
#define LOAD_CONFIG_MENU_ACTION_H

#include "actions/menu_action.h"

class LoadConfigMenuAction : public MenuAction
{
private:
    static const int MAX_CONFIG_FILES = 20;
    static const int MAX_FILENAME_LEN = 32;  // Max length for full path+filename
    char configFiles[MAX_CONFIG_FILES][MAX_FILENAME_LEN]; // Store full filenames with path

    void scanConfigFiles();
    void sortConfigFiles(int count);

public:
    LoadConfigMenuAction(DeviceManager *dev, ActionHandler *hdlr);

    // Implement pure virtual methods from MenuAction
    void onInit() override;
    void onConfirm() override;

    void clear() override;
};

#endif // LOAD_CONFIG_MENU_ACTION_H