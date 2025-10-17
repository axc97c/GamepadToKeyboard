#include "actions/action_handler.h"
#include "actions/action.h"
#include "actions/run_action.h"
#include "actions/main_menu_action.h"
#include "actions/load_config_menu_action.h"
#include "actions/edit_config_menu_action.h"
#include "actions/bind_key_action.h"
#include "actions/text_input_action.h"
#include "actions/save_as_action.h"
#include "actions/stick_config_menu_action.h"
#include "devices.h"

ActionHandler::ActionHandler(DeviceManager *dev)
    : devices(dev), currentAction(nullptr), actionInitialized(false), actionStackSize(0)
{
    // Initialize action stack to null
    for (int i = 0; i < MAX_ACTION_STACK_SIZE; i++)
    {
        actionStack[i] = nullptr;
    }

    // Pre-allocate all action instances ONCE (singleton pattern - no heap churn!)
    Serial.println("ActionHandler: Pre-allocating action instances...");
    runAction = new RunAction(devices, this, {""});
    mainMenuAction = new MainMenuAction(devices, this, {});
    loadConfigMenuAction = new LoadConfigMenuAction(devices, this, {});
    editConfigMenuAction = new EditConfigMenuAction(devices, this, {});
    bindKeyAction = new BindKeyAction(devices, this, {0});
    textInputAction = new TextInputAction(devices, this, {});
    saveAsAction = new SaveAsAction(devices, this);
    stickConfigMenuAction = new StickConfigMenuAction(devices, this, {});
    Serial.println("ActionHandler: Action instances pre-allocated");
}

ActionHandler::~ActionHandler()
{
    Serial.println("ActionHandler: Destructor - cleaning up");

    // Clear current action pointer (don't delete - it's one of our singletons)
    currentAction = nullptr;

    // Clear action stack pointers (don't delete - they're singletons)
    for (int i = 0; i < actionStackSize; i++)
    {
        actionStack[i] = nullptr;
    }
    actionStackSize = 0;

    // Delete pre-allocated action instances
    Serial.println("ActionHandler: Deleting pre-allocated actions...");
    delete runAction;
    delete mainMenuAction;
    delete loadConfigMenuAction;
    delete editConfigMenuAction;
    delete bindKeyAction;
    delete textInputAction;
    delete saveAsAction;
    Serial.println("ActionHandler: Cleanup complete");
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
    // Don't delete - they're singleton instances
    for (int i = 0; i < actionStackSize; i++)
    {
        actionStack[i] = nullptr;
    }
    actionStackSize = 0;

    // Reuse singleton instance
    runAction->setParams(params);
    replaceCurrentAction(runAction);
    Serial.println("ActionHandler: Run mode activated (base action) [SINGLETON]");
}

void ActionHandler::activateMainMenu(MenuActionParams params)
{
    mainMenuAction->setParams(params);
    pushAction(mainMenuAction);
    Serial.println("ActionHandler: Main Menu activated [SINGLETON]");
}

void ActionHandler::activateLoadConfigMenu(MenuActionParams params)
{
    loadConfigMenuAction->setParams(params);
    pushAction(loadConfigMenuAction);
    Serial.println("ActionHandler: Load Config Menu activated [SINGLETON]");
}

void ActionHandler::activateEditConfigMenu(MenuActionParams params)
{
    editConfigMenuAction->setParams(params);
    pushAction(editConfigMenuAction);
    Serial.println("ActionHandler: Edit Config Menu activated [SINGLETON]");
}

void ActionHandler::activateBindKey(BindKeyActionParams params)
{
    bindKeyAction->setParams(params);
    pushAction(bindKeyAction);
    Serial.println("ActionHandler: Bind Key activated [SINGLETON]");
}

void ActionHandler::activateTextInput(TextInputActionParams params)
{
    textInputAction->setParams(params);
    pushAction(textInputAction);
    Serial.println("ActionHandler: Text Input activated [SINGLETON]");
}

void ActionHandler::activateSaveAs()
{
    saveAsAction->reset();
    pushAction(saveAsAction);
    Serial.println("ActionHandler: Save As activated [SINGLETON]");
}

void ActionHandler::activateStickConfigMenu(StickConfigActionParams params)
{
    Serial.print("ActionHandler: StickConfigActionParams.isRight : ");
    Serial.println(params.isRight);
    stickConfigMenuAction->setStickParams(params);
    pushAction(stickConfigMenuAction);
    Serial.println("ActionHandler: Stick Config Menu activated [SINGLETON]");
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
    // Don't delete - actions are singletons
    currentAction = nullptr;
    actionInitialized = false;
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
            Serial.print("ActionHandler: Pushed action onto stack. Stack size: ");
            Serial.println(actionStackSize);
        }
        else
        {
            Serial.println("ActionHandler: ERROR: Action stack full!");
            // Don't delete - actions are singletons!
        }
    }

    // Set the new action as current
    currentAction = action;
    actionInitialized = false;
}

void ActionHandler::replaceCurrentAction(Action *action)
{
    // Replace current action without pushing to stack (for Run action)
    // Don't delete - actions are singletons!
    currentAction = action;
    actionInitialized = false;
}

void ActionHandler::popAction()
{
    // Don't pop if we're at the base (Run action)
    if (actionStackSize == 0)
    {
        Serial.println("ActionHandler: Cannot pop: at base action (Run)");
        return;
    }

    // Don't delete current action - it's a singleton!
    currentAction = nullptr;

    // Pop the previous action from the stack
    actionStackSize--;
    currentAction = actionStack[actionStackSize];
    actionStack[actionStackSize] = nullptr;
    actionInitialized = false;

    Serial.print("ActionHandler: Popped action from stack. Stack size: ");
    Serial.println(actionStackSize);
}

void ActionHandler::popToRunAction()
{
    Serial.println("ActionHandler: Popping all actions back to Run action...");

    // Pop all actions until we're back at the base (Run action)
    while (actionStackSize > 0)
    {
        popAction();
    }

    Serial.println("ActionHandler: Returned to Run action");
}