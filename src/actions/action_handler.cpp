#include "actions/action_handler.h"
#include "actions/action.h"
#include "actions/run_action.h"
#include "actions/example_menu_action.h"
#include "actions/main_menu_action.h"
#include "actions/load_config_menu_action.h"
#include "actions/edit_config_menu_action.h"
#include "devices.h"

ActionHandler::ActionHandler(DeviceManager *dev)
    : devices(dev), currentAction(nullptr), actionInitialized(false), actionStackSize(0)
{
    // Initialize action stack to null
    for (int i = 0; i < MAX_ACTION_STACK_SIZE; i++)
    {
        actionStack[i] = nullptr;
    }
}

ActionHandler::~ActionHandler()
{
    clearAction();

    // Clean up the action stack
    for (int i = 0; i < actionStackSize; i++)
    {
        if (actionStack[i] != nullptr)
        {
            delete actionStack[i];
            actionStack[i] = nullptr;
        }
    }
    actionStackSize = 0;
}

void ActionHandler::setup()
{
    activateRun({"/Default.json"});
}

void ActionHandler::loop()
{
    if (currentAction != nullptr)
    {
        if (!actionInitialized)
        {
            currentAction->init();
            actionInitialized = true;
        }
        currentAction->loop();
    }
}

void ActionHandler::activateRun(RunActionParams params)
{
    // Clear the action stack when activating Run (base action)
    for (int i = 0; i < actionStackSize; i++)
    {
        if (actionStack[i] != nullptr)
        {
            delete actionStack[i];
            actionStack[i] = nullptr;
        }
    }
    actionStackSize = 0;

    strncpy(lastRunFilename, params.filename, sizeof(lastRunFilename) - 1);
    lastRunFilename[sizeof(lastRunFilename) - 1] = '\0';

    // Replace current action without pushing (Run is always at the base)
    replaceCurrentAction(new RunAction(devices, this, params));
    Serial.println((unsigned long)currentAction, HEX);
    Serial.println("Action: Run mode activated (base action)");
}

void ActionHandler::activateMenu(MenuActionParams params)
{
    pushAction(new ExampleMenuAction(devices, this, params));
    Serial.println((unsigned long)currentAction, HEX);
    Serial.println("Action: Example Menu activated");
}

void ActionHandler::activateMainMenu(MenuActionParams params)
{
    pushAction(new MainMenuAction(devices, this, params));
    Serial.println((unsigned long)currentAction, HEX);
    Serial.println("Action: Main Menu activated");
}

void ActionHandler::activateLoadConfigMenu(MenuActionParams params)
{
    pushAction(new LoadConfigMenuAction(devices, this, params));
    Serial.println((unsigned long)currentAction, HEX);
    Serial.println("Action: Load Config Menu activated");
}

void ActionHandler::activateEditConfigMenu(MenuActionParams params)
{
    pushAction(new EditConfigMenuAction(devices, this, params));
    Serial.println((unsigned long)currentAction, HEX);
    Serial.println("Action: Edit Config Menu activated");
}

ActionType ActionHandler::getCurrentActionType()
{
    if (currentAction != nullptr)
    {
        return currentAction->getType();
    }
    return ActionType::NONE;
}

void ActionHandler::clearAction()
{
    if (currentAction != nullptr)
    {
        delete currentAction;
        currentAction = nullptr;
        actionInitialized = false;
    }
}

void ActionHandler::pushAction(Action *action)
{
    // Push the current action onto the stack if it exists
    if (currentAction != nullptr)
    {
        if (actionStackSize < MAX_ACTION_STACK_SIZE)
        {
            actionStack[actionStackSize] = currentAction;
            actionStackSize++;
            Serial.print("Pushed action onto stack. Stack size: ");
            Serial.println(actionStackSize);
        }
        else
        {
            Serial.println("Warning: Action stack full, deleting old action");
            delete currentAction;
        }
    }

    // Set the new action as current
    currentAction = action;
    actionInitialized = false;
}

void ActionHandler::replaceCurrentAction(Action *action)
{
    // Replace current action without pushing to stack (for Run action)
    if (currentAction != nullptr)
    {
        delete currentAction;
    }

    currentAction = action;
    actionInitialized = false;
}

void ActionHandler::popAction()
{
    // Don't pop if we're at the base (Run action)
    if (actionStackSize == 0)
    {
        Serial.println("Cannot pop: at base action (Run)");
        return;
    }

    // Delete the current action
    if (currentAction != nullptr)
    {
        delete currentAction;
        currentAction = nullptr;
    }

    // Pop the previous action from the stack
    actionStackSize--;
    currentAction = actionStack[actionStackSize];
    actionStack[actionStackSize] = nullptr;
    actionInitialized = false;

    Serial.print("Popped action from stack. Stack size: ");
    Serial.println(actionStackSize);
}