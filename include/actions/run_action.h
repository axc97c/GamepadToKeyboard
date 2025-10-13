#ifndef RUN_ACTION_H
#define RUN_ACTION_H

#include "action.h"
#include "action_types.h"


class RunAction : public Action {
  private:
    RunActionParams params;
     
    // Button mappings array
    static const int MAX_MAPPINGS = 128;
    ButtonMapping mappings[MAX_MAPPINGS];
    int numMappings;
    
    float mouseSensitivity;
    int mouseDeadzone;
    int axisCenterValue;
    unsigned long lastMouseUpdate;
    unsigned long mouseUpdateInterval;

    void initializeMappings();
    void initializeDefaultMappings();
    void processButtonMapping(uint8_t buttonNum, bool isPressed);
    void processMouseMovement();
    int applyDeadzone(int value, int deadzone);

  public:
    RunAction(DeviceManager* dev, ActionHandler* hdlr, RunActionParams p);
    
    void init() override;
    void loop() override;
    ActionType getType() override { return ActionType::RUN; }
    
    RunActionParams& getParams() { return params; }
};

#endif