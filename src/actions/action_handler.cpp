#include "actions/action_handler.h"
#include "actions/action.h"
#include "actions/run_action.h"
#include "devices.h"

ActionHandler::ActionHandler(DeviceManager* dev) 
  : devices(dev), currentAction(nullptr), actionInitialized(false) {
}

ActionHandler::~ActionHandler() {
  clearAction();
}

void ActionHandler::setup() {
  // Set default action
  RunActionParams params = {0};
  activateRun(params);
}

void ActionHandler::loop() {
  if (currentAction != nullptr) {
    if (!actionInitialized) {
      currentAction->init();
      actionInitialized = true;
    }
    currentAction->loop();
  }
}

void ActionHandler::activateRun(RunActionParams params) {
  clearAction();
  currentAction = new RunAction(devices, this, params);
  Serial.println((unsigned long)currentAction, HEX);
  actionInitialized = false;
  Serial.println("Action: Run mode activated");
}

ActionType ActionHandler::getCurrentActionType() {
  if (currentAction != nullptr) {
    return currentAction->getType();
  }
  return ActionType::NONE;
}

void ActionHandler::clearAction() {
  if (currentAction != nullptr) {
    delete currentAction;
    currentAction = nullptr;
    actionInitialized = false;
  }
}