#ifndef RUN_ACTION_H
#define RUN_ACTION_H

#include "actions/action.h"
#include "actions/action_types.h"
#include "mapping/joystick_mappings.h"

class RunAction : public Action
{
private:
    RunActionParams params;

    static const int MAX_MAPPINGS = 32;
    ButtonMapping mappings[MAX_MAPPINGS];
    int numMappings;

    // Controller info
    JoystickController::joytype_t controllerType;

    // Stick configurations
    StickConfig leftStick;
    StickConfig rightStick;

    // Mouse/scroll update timing
    unsigned long lastStickUpdate;
    unsigned long stickUpdateInterval;

    // D-pad axis tracking (for PS4/PS5)
    int lastDPadAxisValue;

    void initializeMappings();
    void initializeDefaultMappings();
    void initializeDefaultStickConfigs();

    void processButtonMappings();
    void processDPadAxisMappings();
    void processAnalogStick(StickConfig &stick, int xAxis, int yAxis);

    void processMouseMovement(StickConfig &stick, int xValue, int yValue);
    void processButtonEmulation(StickConfig &stick, int xValue, int yValue);
    void processScrollWheel(StickConfig &stick, int yValue);
    void processWASDKeys(StickConfig &stick, int xValue, int yValue);
    void processArrowKeys(StickConfig &stick, int xValue, int yValue);

    int applyDeadzone(int value, int centerValue, int deadzone);
    const char *getGenericButtonName(uint8_t genericButton);

public:
    RunAction(DeviceManager *dev, ActionHandler *hdlr, RunActionParams p);

    void init() override;
    void loop() override;
    ActionType getType() override;

    RunActionParams &getParams();
};

#endif