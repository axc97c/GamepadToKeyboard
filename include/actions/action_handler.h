#ifndef ACTION_HANDLER_H
#define ACTION_HANDLER_H

#include <Arduino.h>
#include "action_types.h"

class DeviceManager;
class Action;
class RunAction;
class MainMenuAction;
class LoadConfigMenuAction;
class EditConfigMenuAction;
class BindKeyAction;
class TextInputAction;
class SaveAsAction;
class StickConfigMenuAction;
class StickModeMenuAction;
struct TextInputActionParams;

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

    // Pre-allocated action instances (singleton pattern - no heap allocation!)
    RunAction *runAction;
    MainMenuAction *mainMenuAction;
    LoadConfigMenuAction *loadConfigMenuAction;
    EditConfigMenuAction *editConfigMenuAction;
    BindKeyAction *bindKeyAction;
    TextInputAction *textInputAction;
    SaveAsAction *saveAsAction;
    StickConfigMenuAction *stickConfigMenuAction;
    StickModeMenuAction *stickModeMenuAction;

    void pushAction(Action *action);
    void replaceCurrentAction(Action *action);

public:
    ActionHandler(DeviceManager *dev);
    ~ActionHandler();

    // Activate a specific action with parameters
    void activateRun(RunActionParams params);
    void activateMainMenu();
    void activateLoadConfigMenu();
    void activateEditConfigMenu();
    void activateBindKey(BindKeyActionParams params);
    void activateTextInput(TextInputActionParams params);
    void activateSaveAs();
    void activateStickConfigMenu(StickConfigActionParams params);
    void activateStickModeMenu(StickConfigActionParams params);

    // Get current action (returns nullptr if none active)
    Action *getCurrentAction() { return currentAction; }

    const char *getLastRunFilename() { return lastRunFilename; }

    // Navigation
    void popAction();        // Go back to previous action
    void popToRunAction();   // Pop all actions back to Run (base action)

    void setup();
    void loop();
    void clearAction();
};

#endif