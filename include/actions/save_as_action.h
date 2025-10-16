#ifndef SAVE_AS_ACTION_H
#define SAVE_AS_ACTION_H

#include "actions/action.h"

class SaveAsAction : public Action
{
private:
    static const int MAX_FILENAME_LENGTH = 15;
    char filenameBuffer[MAX_FILENAME_LENGTH + 1];
    bool textInputCompleted;
    bool textInputLaunched; // Track if we've already launched text input

public:
    SaveAsAction(DeviceManager *dev, ActionHandler *hdlr);
    virtual ~SaveAsAction();

    // Action lifecycle methods
    void init() override;
    void loop() override;
    ActionType getType() override;

private:
    void performSave();
};

#endif // SAVE_AS_ACTION_H
