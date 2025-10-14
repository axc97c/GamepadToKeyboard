#ifndef ACTIONS_H
#define ACTIONS_H

#include <Arduino.h>
#include "action_types.h"

class DeviceManager;
class ActionHandler;

class Action
{
protected:
    DeviceManager *devices;
    ActionHandler *handler;

public:
    Action(DeviceManager *dev, ActionHandler *hdlr)
        : devices(dev), handler(hdlr)
    {
    }

    virtual ~Action() {}

    // Must be implemented by each action
    virtual void init() = 0;
    virtual void loop() = 0;
    virtual ActionType getType() = 0;
};

#endif