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
    Serial.println("SaveAsAction: init() called");
    Serial.print("SaveAsAction: textInputLaunched: ");
    Serial.println(textInputLaunched);

    // Only launch text input once
    if (!textInputLaunched)
    {
        Serial.println("SaveAsAction: Launching text input...");

        textInputLaunched = true;
        textInputCompleted = false;

        // Launch text input action
        TextInputActionParams params;
        params.prompt = "Filename:";
        params.resultBuffer = filenameBuffer;
        params.maxLength = MAX_FILENAME_LENGTH;

        handler->activateTextInput(params);

        Serial.println("SaveAsAction: Text input launched");
    }
    else
    {
        Serial.println("SaveAsAction: Text input already launched, skipping");
    }
}

void SaveAsAction::loop()
{
    // Check if we've returned from text input (it will have popped itself)
    if (!textInputCompleted && handler->getCurrentAction() == this)
    {
        textInputCompleted = true;

        Serial.print("SaveAsAction: SaveAsAction: Text input returned with: ");
        Serial.println(filenameBuffer);

        // Check if user cancelled (empty filename)
        if (filenameBuffer[0] == '\0')
        {
            Serial.println("SaveAsAction: SaveAsAction: User cancelled");

            // Return to main menu
            handler->popAction();
            return;
        }

        // Perform the save operation
        performSave();
    }
}

ActionType SaveAsAction::getType()
{
    return ActionType::MENU; // Similar to menu action
}

void SaveAsAction::reset()
{
    textInputCompleted = false;
    textInputLaunched = false;
    filenameBuffer[0] = '\0';
    Serial.println("SaveAsAction: reset() called");
}

void SaveAsAction::performSave()
{
    Serial.println("SaveAsAction: performSave START");

    // Build full filename with path and extension using char buffer
    static char fullFilename[64];
    snprintf(fullFilename, sizeof(fullFilename), "/%s.json", filenameBuffer);

    Serial.print("SaveAsAction: SaveAsAction: Saving to: ");
    Serial.println(fullFilename);

    LiquidCrystal_I2C *lcd = devices->getLCD();
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print("Saving as...");
    lcd->setCursor(0, 1);
    lcd->print(filenameBuffer); // Display the name without path/extension

    Serial.println("SaveAsAction: Calling saveConfig...");

    // Save the config
    mappingConfig.setFilename(fullFilename);
    bool success = MappingConfig::saveConfig(fullFilename, mappingConfig);

    Serial.print("SaveAsAction: Save result: ");
    Serial.println(success ? "SUCCESS" : "FAILED");

    lcd->clear();
    lcd->setCursor(0, 0);
    if (success)
    {
        Serial.println("SaveAsAction: Config saved successfully");
        lcd->print("Saved: ");
        lcd->print(filenameBuffer);

        mappingConfig.setFilename(fullFilename);
    }
    else
    {
        Serial.println("SaveAsAction: Save failed");
        lcd->print("Save failed!");
        lcd->setCursor(0, 1);
        lcd->print("Check SD card");
    }

    delay(2000);

    Serial.println("SaveAsAction: Returning to run action");
    handler->popToRunAction();
}
