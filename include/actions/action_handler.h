#ifndef ACTION_HANDLER_H
#define ACTION_HANDLER_H

#include <Arduino.h>
#include "action_types.h"

class DeviceManager;
class Action;

class ActionHandler {
  private:
    DeviceManager* devices;
    Action* currentAction;
    bool actionInitialized;
    
  public:
    ActionHandler(DeviceManager* dev);
    ~ActionHandler();
    
    // Activate a specific action with parameters
    void activateRun(RunActionParams params);
    //void activateMenuDisplay(MenuDisplayParams params);
    //void activateKeyMappingConfig(KeyMappingConfigParams params);
    
    // Get current action (returns nullptr if none active)
    Action* getCurrentAction() { return currentAction; }
    ActionType getCurrentActionType();
    
    void setup();
    void loop();
    void clearAction();
};

#endif