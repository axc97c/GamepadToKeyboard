#include "devices.h"

DeviceManager::DeviceManager() 
  : host(nullptr), hub(nullptr), keyboard(nullptr), 
    mouse(nullptr), joystick(nullptr) {
}

DeviceManager::~DeviceManager() {
}

void DeviceManager::setup() {
    host->begin();
}

void DeviceManager::loop() {
    host->Task();
}