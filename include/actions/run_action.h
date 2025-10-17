#ifndef RUN_ACTION_H
#define RUN_ACTION_H

#include "actions/action.h"
#include "actions/action_types.h"
#include "mapping/joystick_mappings.h"

class RunAction : public Action
{
private:
    RunActionParams params;

    // Controller info
    JoystickController::joytype_t controllerType;

    // Mouse/scroll update timing
    unsigned long lastStickUpdate;
    unsigned long stickUpdateInterval;
    unsigned long lastTriggerUpdate;
    unsigned long backlightOnTime;
    static const unsigned long BACKLIGHT_TIMEOUT_MS = 15000;

    // D-pad axis tracking (for PS4/PS5)
    int lastDPadAxisValue;

    // Keyboard passthrough event handlers
    static void onKeyPress(int unicode);
    static void onKeyRelease(int unicode);
    static void onExtrasPress(uint32_t top, uint16_t key);
    static void onExtrasRelease(uint32_t top, uint16_t key);

    void initializeMappings();
    void initializeDefaultMappings();
    void initializeDefaultStickConfigs();
    void initializeDefaultTriggerConfigs();
    
    void processButtonMappings();
    void processDPadAxisMappings();
    void processAnalogStick(StickConfig &stick, int xAxis, int yAxis);

    void processMouseMovement(StickConfig &stick, int xValue, int yValue);
    void processButtonEmulation(StickConfig &stick, int xValue, int yValue);
    void processScrollWheel(StickConfig &stick, int yValue);
    void processWASDKeys(StickConfig &stick, int xValue, int yValue);
    void processArrowKeys(StickConfig &stick, int xValue, int yValue);

    void processTriggers(int leftAxis, int rightAxis);
    void processTriggerButtons(int leftValue, int rightValue);
    void processTriggerMouseX(int leftValue, int rightValue);
    void processTriggerMouseY(int leftValue, int rightValue);
    void processTriggerScroll(int leftValue, int rightValue);

    int applyDeadzone(int value, int centerValue, int deadzone);
    const char *getGenericButtonName(uint8_t genericButton);

    void DisplayLoadedFile();

public:
    RunAction(DeviceManager *dev, ActionHandler *hdlr, RunActionParams p);

    void init() override;
    void loop() override;

    void setParams(RunActionParams p);  // For singleton reuse
};

#endif