#ifndef ACTION_HANDLER_H
#define ACTION_HANDLER_H

#include <Arduino.h>
#include "action_types.h"

class DeviceManager;
class Action;

class ActionHandler
{
private:
    static const int MAX_ACTION_STACK_SIZE = 16;

    DeviceManager *devices;
    Action *currentAction;
    bool actionInitialized;
    char lastRunFilename[64];

    // Action stack for navigation
    Action *actionStack[MAX_ACTION_STACK_SIZE];
    int actionStackSize;

    void pushAction(Action *action);
    void replaceCurrentAction(Action *action);

public:
    ActionHandler(DeviceManager *dev);
    ~ActionHandler();

    // Activate a specific action with parameters
    void activateRun(RunActionParams params);
    void activateMainMenu(MenuActionParams params);
    void activateLoadConfigMenu(MenuActionParams params);
    void activateEditConfigMenu(MenuActionParams params);

    // Get current action (returns nullptr if none active)
    Action *getCurrentAction() { return currentAction; }
    ActionType getCurrentActionType();

    const char *getLastRunFilename() { return lastRunFilename; }

    // Navigation
    void popAction();        // Go back to previous action
    void popToRunAction();   // Pop all actions back to Run (base action)

    void setup();
    void loop();
    void clearAction();
};

#endif