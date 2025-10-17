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
#include "actions/stick_mode_menu_action.h"
#include "actions/trigger_config_menu_action.h"
#include "actions/trigger_mode_menu_action.h"
#include "devices.h"

ActionHandler::ActionHandler(DeviceManager *dev)
    : devices(dev), currentAction(nullptr), actionInitialized(false), actionStackSize(0)
{
    for (int i = 0; i < MAX_ACTION_STACK_SIZE; i++)
    {
        actionStack[i] = nullptr;
    }

    runAction = new RunAction(devices, this, {""});
    mainMenuAction = new MainMenuAction(devices, this);
    loadConfigMenuAction = new LoadConfigMenuAction(devices, this);
    editConfigMenuAction = new EditConfigMenuAction(devices, this);
    bindKeyAction = new BindKeyAction(devices, this, {0});
    textInputAction = new TextInputAction(devices, this, {});
    saveAsAction = new SaveAsAction(devices, this);
    stickConfigMenuAction = new StickConfigMenuAction(devices, this, {});
    stickModeMenuAction = new StickModeMenuAction(devices, this, {});
    triggerConfigMenuAction = new TriggerConfigMenuAction(devices, this);
    triggerModeMenuAction = new TriggerModeMenuAction(devices, this);
}

ActionHandler::~ActionHandler()
{
    currentAction = nullptr;

    for (int i = 0; i < actionStackSize; i++)
    {
        actionStack[i] = nullptr;
    }
    actionStackSize = 0;

    delete runAction;
    delete mainMenuAction;
    delete loadConfigMenuAction;
    delete editConfigMenuAction;
    delete bindKeyAction;
    delete textInputAction;
    delete saveAsAction;
    delete stickConfigMenuAction;
    delete stickModeMenuAction;   
    delete triggerConfigMenuAction;
    delete triggerModeMenuAction;
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
    for (int i = 0; i < actionStackSize; i++)
    {
        actionStack[i] = nullptr;
    }
    actionStackSize = 0;

    runAction->setParams(params);
    replaceCurrentAction(runAction);
}

void ActionHandler::activateMainMenu()
{
    pushAction(mainMenuAction);
}

void ActionHandler::activateLoadConfigMenu()
{
    pushAction(loadConfigMenuAction);
}

void ActionHandler::activateEditConfigMenu()
{
    pushAction(editConfigMenuAction);
}

void ActionHandler::activateBindKey(BindKeyActionParams params)
{
    bindKeyAction->setParams(params);
    pushAction(bindKeyAction);
}

void ActionHandler::activateTextInput(TextInputActionParams params)
{
    textInputAction->setParams(params);
    pushAction(textInputAction);
}

void ActionHandler::activateSaveAs()
{
    saveAsAction->reset();
    pushAction(saveAsAction);
}

void ActionHandler::activateStickConfigMenu(StickConfigActionParams params)
{
    stickConfigMenuAction->setStickParams(params);
    pushAction(stickConfigMenuAction);
}

void ActionHandler::activateStickModeMenu(StickConfigActionParams params)
{
    stickModeMenuAction->setStickParams(params);
    pushAction(stickModeMenuAction);
}

void ActionHandler::activateTriggerConfigMenu()
{
    pushAction(triggerConfigMenuAction);
}

void ActionHandler::activateTriggerModeMenu()
{
    pushAction(triggerModeMenuAction);
}

void ActionHandler::clearAction()
{
    currentAction = nullptr;
    actionInitialized = false;
}

void ActionHandler::pushAction(Action *action)
{
    if (currentAction != nullptr)
    {
        if (actionStackSize < MAX_ACTION_STACK_SIZE)
        {
            actionStack[actionStackSize] = currentAction;
            actionStackSize++;
        }
    }

    currentAction = action;
    actionInitialized = false;
}

void ActionHandler::replaceCurrentAction(Action *action)
{
    currentAction = action;
    actionInitialized = false;
}

void ActionHandler::popAction()
{
    if (actionStackSize == 0)
    {
        return;
    }

    currentAction = nullptr;

    actionStackSize--;
    currentAction = actionStack[actionStackSize];
    actionStack[actionStackSize] = nullptr;
    actionInitialized = false;
}

void ActionHandler::popToRunAction()
{
    while (actionStackSize > 0)
    {
        popAction();
    }
}