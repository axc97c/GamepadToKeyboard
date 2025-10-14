#ifndef ACTION_HANDLER_H
#define ACTION_HANDLER_H

#include <Arduino.h>
#include "action_types.h"

class DeviceManager;
class Action;

class ActionHandler
{
private:
    DeviceManager *devices;
    Action *currentAction;
    bool actionInitialized;
    char lastRunFilename[64];
public:
    ActionHandler(DeviceManager *dev);
    ~ActionHandler();

    // Activate a specific action with parameters
    void activateRun(RunActionParams params);
    void activateMenu(MenuActionParams params);
    void activateMainMenu(MenuActionParams params);
    void activateLoadConfigMenu(MenuActionParams params);
    void activateEditConfigMenu(MenuActionParams params);

    // Get current action (returns nullptr if none active)
    Action *getCurrentAction() { return currentAction; }
    ActionType getCurrentActionType();

    const char *getLastRunFilename() { return lastRunFilename; }

    void setup();
    void loop();
    void clearAction();
};

#endif