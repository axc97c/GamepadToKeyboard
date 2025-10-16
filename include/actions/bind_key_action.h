#ifndef BIND_KEY_ACTION_H
#define BIND_KEY_ACTION_H

#include "actions/action.h"
#include "input/keyboard_input.h"

class BindKeyAction : public Action
{
private:
    BindKeyActionParams params;

    // Display state
    bool displayInitialized;
    unsigned long lastDisplayUpdate;
    const unsigned long displayUpdateInterval = 500; // For blinking cursor effect

    void updateDisplay();
    void applyKeyBinding(int keyCode);

public:
    BindKeyAction(DeviceManager *dev, ActionHandler *hdlr, BindKeyActionParams p);
    virtual ~BindKeyAction();

    // Action lifecycle methods
    void init() override;
    void loop() override;
    ActionType getType() override;
    void setParams(BindKeyActionParams p);  // For singleton reuse
};

#endif // BIND_KEY_ACTION_H
