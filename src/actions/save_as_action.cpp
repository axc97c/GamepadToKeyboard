#include "actions/save_as_action.h"
#include "actions/action_handler.h"
#include "actions/text_input_action.h"
#include "mapping/mapping_config.h"
#include "devices.h"
#include "utils.h"

SaveAsAction::SaveAsAction(DeviceManager *dev, ActionHandler *hdlr)
    : Action(dev, hdlr), textInputCompleted(false), textInputLaunched(false)
{
    filenameBuffer[0] = '\0';
}

SaveAsAction::~SaveAsAction()
{
}

void SaveAsAction::init()
{
    if (!textInputLaunched)
    {
        textInputLaunched = true;
        textInputCompleted = false;

        TextInputActionParams params;
        params.prompt = "Filename:";
        params.resultBuffer = filenameBuffer;
        params.maxLength = MAX_FILENAME_LENGTH;

        handler->activateTextInput(params);
    }
}

void SaveAsAction::loop()
{
    // Check if we've returned from text input (it will have popped itself)
    if (!textInputCompleted && handler->getCurrentAction() == this)
    {
        textInputCompleted = true;

        if (filenameBuffer[0] == '\0')
        {
            handler->popAction();
            return;
        }

        performSave();
    }
}

void SaveAsAction::reset()
{
    textInputCompleted = false;
    textInputLaunched = false;
    filenameBuffer[0] = '\0';
}

void SaveAsAction::performSave()
{
    static char fullFilename[64];
    snprintf(fullFilename, sizeof(fullFilename), "/%s.json", filenameBuffer);

    LiquidCrystal_I2C *lcd = devices->getLCD();
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print("Saving as...");
    lcd->setCursor(0, 1);
    lcd->print(filenameBuffer); // Display the name without path/extension

    mappingConfig.setFilename(fullFilename);
    bool success = MappingConfig::saveConfig(fullFilename, mappingConfig);

    lcd->clear();
    lcd->setCursor(0, 0);
    if (success)
    {
        lcd->print("Saved: ");
        lcd->print(filenameBuffer);

        mappingConfig.setFilename(fullFilename);
    }
    else
    {
        lcd->print("Save failed!");
        lcd->setCursor(0, 1);
        lcd->print("Check SD card");
    }

    delay(1000);

    handler->popToRunAction();
}
