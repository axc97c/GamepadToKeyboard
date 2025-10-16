#include "actions/save_as_action.h"
#include "actions/action_handler.h"
#include "actions/text_input_action.h"
#include "mapping/mapping_config.h"
#include "devices.h"
#include "utils.h"

SaveAsAction::SaveAsAction(DeviceManager *dev, ActionHandler *hdlr)
    : Action(dev, hdlr), textInputCompleted(false)
{
    filenameBuffer[0] = '\0';
}

SaveAsAction::~SaveAsAction()
{
}

void SaveAsAction::init()
{
    Serial.println("SaveAsAction: Initializing...");

    // Launch text input action
    TextInputActionParams params;
    params.prompt = "Filename:";
    params.resultBuffer = filenameBuffer;
    params.maxLength = MAX_FILENAME_LENGTH;

    handler->activateTextInput(params);
    textInputCompleted = false;

    Serial.println("SaveAsAction: Text input action launched");
}

void SaveAsAction::loop()
{
    // Check if we've returned from text input (it will have popped itself)
    if (!textInputCompleted && handler->getCurrentAction() == this)
    {
        textInputCompleted = true;

        Serial.print("SaveAsAction: Text input returned with: ");
        Serial.println(filenameBuffer);

        // Check if user cancelled (empty filename)
        if (filenameBuffer[0] == '\0')
        {
            Serial.println("SaveAsAction: User cancelled");

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

void SaveAsAction::performSave()
{
    // Build full filename with path and extension
    String fullFilename = "/" + String(filenameBuffer) + ".json";

    Serial.print("SaveAsAction: Saving config to: ");
    Serial.println(fullFilename);

    LiquidCrystal_I2C *lcd = devices->getLCD();
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print("Saving as...");
    lcd->setCursor(0, 1);

    // Display filename (may need to truncate for display)
    String displayName = Utils::trimFilename(fullFilename.c_str());
    lcd->print(displayName);

    // Save the config
    bool success = MappingConfig::saveConfig(fullFilename.c_str(), mappingConfig);

    lcd->clear();
    lcd->setCursor(0, 0);
    if (success)
    {
        Serial.println("SaveAsAction: Config saved successfully");
        lcd->print("Saved as:");
        lcd->setCursor(0, 1);
        lcd->print(displayName);

        // Update the current config filename
        strncpy(mappingConfig.filename, fullFilename.c_str(), JoystickMappingConfig::MAX_FILENAME_LENGTH - 1);
        mappingConfig.filename[JoystickMappingConfig::MAX_FILENAME_LENGTH - 1] = '\0';

        Serial.print("SaveAsAction: Updated mappingConfig.filename to: ");
        Serial.println(mappingConfig.filename);
    }
    else
    {
        Serial.println("SaveAsAction: Failed to save config");
        lcd->print("Save failed!");
        lcd->setCursor(0, 1);
        lcd->print("Check SD card");
    }

    delay(2000);

    // Return to run action
    Serial.println("SaveAsAction: Returning to run action");
    handler->popToRunAction();
}
