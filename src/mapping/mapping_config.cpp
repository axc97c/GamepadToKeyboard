#include "mapping/mapping_config.h"
#include "mapping/joystick_mappings.h"
#include "mapping/keyboard_mapping.h"

void MappingConfig::initSD() {
    if (!SD.begin(CHIPSELECT_PIN)) {
        Serial.println("SD Card initialization failed!");
        return;
    }
    Serial.println("SD Card initialized successfully");
}

bool MappingConfig::loadConfig(const char* filename, ButtonMapping* mappings, int& numMappings, int maxMappings, StickConfig* leftStick, StickConfig* rightStick)
{
    File file = SD.open(filename, FILE_READ);
    if (!file) {
        Serial.print("Failed to open file: ");
        Serial.println(filename);
        return false;
    }
    
    Serial.print("Reading mappings from: ");
    Serial.println(filename);
    
    JsonDocument doc;
    
    DeserializationError error = deserializeJson(doc, file);
    file.close();
    
    if (error) {
        Serial.print("JSON parsing failed: ");
        Serial.println(error.c_str());
        return false;
    }
    
    loadMappings(doc, mappings, numMappings, maxMappings);
    loadStickConfig(doc, leftStick, rightStick);

    return true;
}

void MappingConfig::loadMappings(JsonDocument doc, ButtonMapping* mappings, int& numMappings, int maxMappings) {

    JsonArray mappingsArray = doc["mappings"].as<JsonArray>();
    numMappings = 0;
    
    for (JsonObject mapping : mappingsArray) {
        if (numMappings >= maxMappings) {
            Serial.println("Warning: Too many mappings, truncating");
            break;
        }
        
        const char* buttonStr = mapping["button"];
        const char* keyStr = mapping["key"];
        
        int genericButton = ControllerMapper::parseGenericButtonName(buttonStr);
        int keyCode = KeyboardMapping::parseKeyCode(keyStr);
        
        if (genericButton != -1 && keyCode != -1) {
            mappings[numMappings].genericButton = genericButton;
            mappings[numMappings].keyCode = keyCode;
            mappings[numMappings].currentlyPressed = false;
            
            Serial.print("  Loaded: ");
            Serial.print(buttonStr);
            Serial.print(" -> ");
            Serial.println(keyStr);
            
            numMappings++;
        }
    }
    
    Serial.print("Loaded ");
    Serial.print(numMappings);
    Serial.println(" mappings");
}

void MappingConfig::loadStickConfig(JsonDocument doc, StickConfig* leftStick, StickConfig* rightStick) {

    // Load left stick config
    if (doc["leftStick"].is<JsonObject>()) {
        JsonObject left = doc["leftStick"];
        parseStickConfig(leftStick, left);

        Serial.print("  Left stick: ");
        Serial.println(stickBehaviorToString(leftStick->behavior));
    }
    
    // Load right stick config
    if (doc["rightStick"].is<JsonObject>()) {
        JsonObject right = doc["rightStick"];   
        parseStickConfig(rightStick, right);

        Serial.print("  Right stick: ");
        Serial.println(stickBehaviorToString(rightStick->behavior));
    }
    
    Serial.println("Stick configuration loaded");
}

void MappingConfig::parseStickConfig(StickConfig *stickConfig, ArduinoJson::V742PB22::JsonObject &jsonObject)
{
    stickConfig->behavior = parseStickBehavior(jsonObject["behavior"]);
    stickConfig->sensitivity = jsonObject["sensitivity"] | 0.15f;
    stickConfig->deadzone = jsonObject["deadzone"] | 16;
    stickConfig->activationThreshold = jsonObject["activationThreshold"] | 64;

    if (jsonObject["keys"].is<JsonObject>())
    {
        JsonObject keys = jsonObject["keys"];
        stickConfig->keyUp = KeyboardMapping::parseKeyCode(keys["up"]);
        stickConfig->keyDown = KeyboardMapping::parseKeyCode(keys["down"]);
        stickConfig->keyLeft = KeyboardMapping::parseKeyCode(keys["left"]);
        stickConfig->keyRight = KeyboardMapping::parseKeyCode(keys["right"]);
    }
}

bool MappingConfig::saveConfig(const char* filename, ButtonMapping* mappings, int& numMappings, StickConfig* leftStick, StickConfig* rightStick)
{
    if (SD.exists(filename)) {
        SD.remove(filename);
    }
    
    File file = SD.open(filename, FILE_WRITE);
    if (!file) {
        Serial.print("Failed to create file: ");
        Serial.println(filename);
        return false;
    }
    
    JsonDocument doc;

    saveMappings(doc, mappings, numMappings);
    saveStickConfig(doc, leftStick, rightStick);

    // Write to file
    if (SD.exists(filename)) {
        SD.remove(filename);
    }
    
    file = SD.open(filename, FILE_WRITE);
    if (!file) {
        Serial.println("Failed to save stick config");
        return false;
    }
    
    serializeJsonPretty(doc, file);
    file.close();
    
    return true;
}

bool MappingConfig::saveMappings(JsonDocument doc, ButtonMapping* mappings, int numMappings) {
    JsonArray mappingsArray = doc["mappings"].to<JsonArray>();
    
    for (int i = 0; i < numMappings; i++) {
        JsonObject mapping = mappingsArray.add<JsonObject>();
        mapping["button"] = ControllerMapper::getGenericButtonName(mappings[i].genericButton);
        mapping["key"] = KeyboardMapping::keyCodeToString(mappings[i].keyCode);
    }
    
    Serial.print("Saved ");
    Serial.print(numMappings);
    Serial.println(" mappings");
    
    return true;
}

bool MappingConfig::saveStickConfig(JsonDocument doc, StickConfig* leftStick, StickConfig* rightStick) {
    // Update stick configs
    JsonObject left = doc["leftStick"].to<JsonObject>();
    left["behavior"] = stickBehaviorToString(leftStick->behavior);
    left["sensitivity"] = leftStick->sensitivity;
    left["deadzone"] = leftStick->deadzone;
    left["activationThreshold"] = leftStick->activationThreshold;
    
    if (leftStick->behavior == StickBehavior::CUSTOM_KEYS || 
        leftStick->behavior == StickBehavior::BUTTON_EMULATION) {
        JsonObject leftKeys = left["keys"].to<JsonObject>();
        leftKeys["up"] = KeyboardMapping::keyCodeToString(leftStick->keyUp);
        leftKeys["down"] = KeyboardMapping::keyCodeToString(leftStick->keyDown);
        leftKeys["left"] = KeyboardMapping::keyCodeToString(leftStick->keyLeft);
        leftKeys["right"] = KeyboardMapping::keyCodeToString(leftStick->keyRight);
    }
    
    JsonObject right = doc["rightStick"].to<JsonObject>();
    right["behavior"] = stickBehaviorToString(rightStick->behavior);
    right["sensitivity"] = rightStick->sensitivity;
    right["deadzone"] = rightStick->deadzone;
    right["activationThreshold"] = rightStick->activationThreshold;
    
    if (rightStick->behavior == StickBehavior::CUSTOM_KEYS || 
        rightStick->behavior == StickBehavior::BUTTON_EMULATION) {
        JsonObject rightKeys = right["keys"].to<JsonObject>();
        rightKeys["up"] = KeyboardMapping::keyCodeToString(rightStick->keyUp);
        rightKeys["down"] = KeyboardMapping::keyCodeToString(rightStick->keyDown);
        rightKeys["left"] = KeyboardMapping::keyCodeToString(rightStick->keyLeft);
        rightKeys["right"] = KeyboardMapping::keyCodeToString(rightStick->keyRight);
    }
  
    Serial.println("Stick configuration saved");
    return true;
}

StickBehavior MappingConfig::parseStickBehavior(const char* behaviorStr) {
    if (strcmp(behaviorStr, "DISABLED") == 0) return StickBehavior::DISABLED;
    if (strcmp(behaviorStr, "MOUSE_MOVEMENT") == 0) return StickBehavior::MOUSE_MOVEMENT;
    if (strcmp(behaviorStr, "BUTTON_EMULATION") == 0) return StickBehavior::BUTTON_EMULATION;
    if (strcmp(behaviorStr, "SCROLL_WHEEL") == 0) return StickBehavior::SCROLL_WHEEL;
    if (strcmp(behaviorStr, "WASD_KEYS") == 0) return StickBehavior::WASD_KEYS;
    if (strcmp(behaviorStr, "ARROW_KEYS") == 0) return StickBehavior::ARROW_KEYS;
    if (strcmp(behaviorStr, "CUSTOM_KEYS") == 0) return StickBehavior::CUSTOM_KEYS;
    
    Serial.print("Warning: Unknown stick behavior: ");
    Serial.println(behaviorStr);
    return StickBehavior::DISABLED;
}

const char* MappingConfig::stickBehaviorToString(StickBehavior behavior) {
    switch(behavior) {
        case StickBehavior::DISABLED: return "DISABLED";
        case StickBehavior::MOUSE_MOVEMENT: return "MOUSE_MOVEMENT";
        case StickBehavior::BUTTON_EMULATION: return "BUTTON_EMULATION";
        case StickBehavior::SCROLL_WHEEL: return "SCROLL_WHEEL";
        case StickBehavior::WASD_KEYS: return "WASD_KEYS";
        case StickBehavior::ARROW_KEYS: return "ARROW_KEYS";
        case StickBehavior::CUSTOM_KEYS: return "CUSTOM_KEYS";
        default: return "DISABLED";
    }
}
