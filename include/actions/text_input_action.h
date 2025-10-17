#ifndef TEXT_INPUT_ACTION_H
#define TEXT_INPUT_ACTION_H

#include "actions/action.h"
#include "input/keyboard_input.h"

struct TextInputActionParams
{
    const char *prompt;     // Text to display as prompt
    char *resultBuffer;     // Buffer to store the result
    unsigned int maxLength;          // Maximum length of input (excluding null terminator)
};

class TextInputAction : public Action
{
private:
    TextInputActionParams params;

    String inputText;
    int cursorPosition;

    // Display state
    unsigned long lastBlinkTime;
    bool cursorVisible;
    const unsigned long blinkInterval = 500;

    void updateDisplay();
    void handleKeyPress(int unicode);
    void finishInput();
    void cancelInput();

public:
    TextInputAction(DeviceManager *dev, ActionHandler *hdlr, TextInputActionParams p);
    virtual ~TextInputAction();

    // Action lifecycle methods
    void init() override;
    void loop() override;
    void setParams(TextInputActionParams p);  // For singleton reuse
};

#endif // TEXT_INPUT_ACTION_H
