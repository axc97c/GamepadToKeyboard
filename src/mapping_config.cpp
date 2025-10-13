#include "mapping_config.h"
#include "actions/run_action.h"
#include "joystick_mappings.h" 

// Add this function to parse Xbox button names
int MappingConfig::parseButtonName(const char* buttonStr) {
    // Xbox 360 button names
    if (strcmp(buttonStr, "LB") == 0) return Xbox360::LB;
    if (strcmp(buttonStr, "RB") == 0) return Xbox360::RB;
    if (strcmp(buttonStr, "XBOX") == 0 || strcmp(buttonStr, "XBOX_BUTTON") == 0) return Xbox360::XBOX_BUTTON;
    if (strcmp(buttonStr, "A") == 0) return Xbox360::A;
    if (strcmp(buttonStr, "B") == 0) return Xbox360::B;
    if (strcmp(buttonStr, "X") == 0) return Xbox360::X;
    if (strcmp(buttonStr, "Y") == 0) return Xbox360::Y;
    if (strcmp(buttonStr, "DPAD_UP") == 0) return Xbox360::DPAD_UP;
    if (strcmp(buttonStr, "DPAD_DOWN") == 0) return Xbox360::DPAD_DOWN;
    if (strcmp(buttonStr, "DPAD_LEFT") == 0) return Xbox360::DPAD_LEFT;
    if (strcmp(buttonStr, "DPAD_RIGHT") == 0) return Xbox360::DPAD_RIGHT;
    if (strcmp(buttonStr, "START") == 0) return Xbox360::START;
    if (strcmp(buttonStr, "BACK") == 0) return Xbox360::BACK;
    if (strcmp(buttonStr, "LEFT_STICK") == 0 || strcmp(buttonStr, "L3") == 0) return Xbox360::LEFT_STICK;
    if (strcmp(buttonStr, "RIGHT_STICK") == 0 || strcmp(buttonStr, "R3") == 0) return Xbox360::RIGHT_STICK;
    
    Serial.print("Warning: Unknown button name: ");
    Serial.println(buttonStr);
    return -1;
}

const char* MappingConfig::buttonNameToString(uint8_t buttonNum) {
    switch(buttonNum) {
        case Xbox360::LB: return "LB";
        case Xbox360::RB: return "RB";
        case Xbox360::XBOX_BUTTON: return "XBOX";
        case Xbox360::A: return "A";
        case Xbox360::B: return "B";
        case Xbox360::X: return "X";
        case Xbox360::Y: return "Y";
        case Xbox360::DPAD_UP: return "DPAD_UP";
        case Xbox360::DPAD_DOWN: return "DPAD_DOWN";
        case Xbox360::DPAD_LEFT: return "DPAD_LEFT";
        case Xbox360::DPAD_RIGHT: return "DPAD_RIGHT";
        case Xbox360::START: return "START";
        case Xbox360::BACK: return "BACK";
        case Xbox360::LEFT_STICK: return "LEFT_STICK";
        case Xbox360::RIGHT_STICK: return "RIGHT_STICK";
        default: return "UNKNOWN";
    }
}


void MappingConfig::initSD() {
    if (!SD.begin(CHIPSELECT_PIN)) {
        Serial.println("SD Card initialization failed!");
        return;
    }
    Serial.println("SD Card initialized successfully");
}

bool MappingConfig::loadMappings(const char* filename, ButtonMapping* mappings, int& numMappings, int maxMappings) {
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
    
    JsonArray mappingsArray = doc["mappings"].as<JsonArray>();
    numMappings = 0;
    
    for (JsonObject mapping : mappingsArray) {
        if (numMappings >= maxMappings) {
            Serial.println("Warning: Too many mappings, truncating");
            break;
        }
        
        const char* buttonStr = mapping["button"];
        const char* keyStr = mapping["key"];
        
        int buttonNum = parseButtonName(buttonStr);
        int keyCode = parseKeyCode(keyStr);
        
        if (buttonNum != -1 && keyCode != -1) {
            mappings[numMappings].joystickButton = buttonNum;
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
    
    return true;
}

bool MappingConfig::saveMappings(const char* filename, ButtonMapping* mappings, int numMappings) {
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
    JsonArray mappingsArray = doc["mappings"].to<JsonArray>();
    
    for (int i = 0; i < numMappings; i++) {
        JsonObject mapping = mappingsArray.add<JsonObject>();
        mapping["button"] = buttonNameToString(mappings[i].joystickButton);
        mapping["key"] = keyCodeToString(mappings[i].keyCode);
    }
    
    serializeJsonPretty(doc, file);
    file.close();
    
    Serial.print("Saved ");
    Serial.print(numMappings);
    Serial.println(" mappings");
    
    return true;
}

int MappingConfig::parseKeyCode(const char* keyStr) {
    // Single characters
    if (strlen(keyStr) == 1) {
        return keyStr[0];
    }
    
    // Special keys
    if (strcmp(keyStr, "RETURN") == 0 || strcmp(keyStr, "ENTER") == 0) return KEY_RETURN;
    if (strcmp(keyStr, "ESC") == 0 || strcmp(keyStr, "ESCAPE") == 0) return KEY_ESC;
    if (strcmp(keyStr, "BACKSPACE") == 0) return KEY_BACKSPACE;
    if (strcmp(keyStr, "TAB") == 0) return KEY_TAB;
    if (strcmp(keyStr, "SPACE") == 0) return ' ';
    
    // Modifiers
    if (strcmp(keyStr, "LEFT_CTRL") == 0) return KEY_LEFT_CTRL;
    if (strcmp(keyStr, "LEFT_SHIFT") == 0) return KEY_LEFT_SHIFT;
    if (strcmp(keyStr, "LEFT_ALT") == 0) return KEY_LEFT_ALT;
    if (strcmp(keyStr, "LEFT_GUI") == 0) return KEY_LEFT_GUI;
    if (strcmp(keyStr, "RIGHT_CTRL") == 0) return KEY_RIGHT_CTRL;
    if (strcmp(keyStr, "RIGHT_SHIFT") == 0) return KEY_RIGHT_SHIFT;
    if (strcmp(keyStr, "RIGHT_ALT") == 0) return KEY_RIGHT_ALT;
    if (strcmp(keyStr, "RIGHT_GUI") == 0) return KEY_RIGHT_GUI;
    
    // Arrow keys
    if (strcmp(keyStr, "UP") == 0) return KEY_UP;
    if (strcmp(keyStr, "DOWN") == 0) return KEY_DOWN;
    if (strcmp(keyStr, "LEFT") == 0) return KEY_LEFT;
    if (strcmp(keyStr, "RIGHT") == 0) return KEY_RIGHT;
    
    // Function keys
    if (strcmp(keyStr, "F1") == 0) return KEY_F1;
    if (strcmp(keyStr, "F2") == 0) return KEY_F2;
    if (strcmp(keyStr, "F3") == 0) return KEY_F3;
    if (strcmp(keyStr, "F4") == 0) return KEY_F4;
    if (strcmp(keyStr, "F5") == 0) return KEY_F5;
    if (strcmp(keyStr, "F6") == 0) return KEY_F6;
    if (strcmp(keyStr, "F7") == 0) return KEY_F7;
    if (strcmp(keyStr, "F8") == 0) return KEY_F8;
    if (strcmp(keyStr, "F9") == 0) return KEY_F9;
    if (strcmp(keyStr, "F10") == 0) return KEY_F10;
    if (strcmp(keyStr, "F11") == 0) return KEY_F11;
    if (strcmp(keyStr, "F12") == 0) return KEY_F12;
    
    // Other keys
    if (strcmp(keyStr, "INSERT") == 0) return KEY_INSERT;
    if (strcmp(keyStr, "DELETE") == 0) return KEY_DELETE;
    if (strcmp(keyStr, "HOME") == 0) return KEY_HOME;
    if (strcmp(keyStr, "END") == 0) return KEY_END;
    if (strcmp(keyStr, "PAGE_UP") == 0) return KEY_PAGE_UP;
    if (strcmp(keyStr, "PAGE_DOWN") == 0) return KEY_PAGE_DOWN;
    
    Serial.print("Warning: Unknown key string: ");
    Serial.println(keyStr);
    return -1;
}

const char* MappingConfig::keyCodeToString(int keyCode) {
    // Special keys
    if (keyCode == KEY_RETURN) return "RETURN";
    if (keyCode == KEY_ESC) return "ESC";
    if (keyCode == KEY_BACKSPACE) return "BACKSPACE";
    if (keyCode == KEY_TAB) return "TAB";
    if (keyCode == ' ') return "SPACE";
    
    // Modifiers
    if (keyCode == KEY_LEFT_CTRL) return "LEFT_CTRL";
    if (keyCode == KEY_LEFT_SHIFT) return "LEFT_SHIFT";
    if (keyCode == KEY_LEFT_ALT) return "LEFT_ALT";
    if (keyCode == KEY_LEFT_GUI) return "LEFT_GUI";
    
    // Arrow keys
    if (keyCode == KEY_UP) return "UP";
    if (keyCode == KEY_DOWN) return "DOWN";
    if (keyCode == KEY_LEFT) return "LEFT";
    if (keyCode == KEY_RIGHT) return "RIGHT";
    
    // Single character - return as string
    static char buf[2];
    buf[0] = (char)keyCode;
    buf[1] = '\0';
    return buf;
}