#include "mapping/mapping_config.h"
#include "mapping/joystick_mappings.h"
#include "mapping/keyboard_mapping.h"

const MappingConfig::StickBehaviorMapping MappingConfig::stickBehaviorMap[] = {
    {StickBehavior::DISABLED, "Disabled"},
    {StickBehavior::MOUSE_MOVEMENT, "Mouse"},
    {StickBehavior::BUTTON_EMULATION, "Custom Keys"},
    {StickBehavior::SCROLL_WHEEL, "Scroll"},
    {StickBehavior::WASD_KEYS, "WASD Keys"},
    {StickBehavior::ARROW_KEYS, "Arrow Keys"}
};

const int MappingConfig::stickBehaviorMapSize = sizeof(MappingConfig::stickBehaviorMap) / sizeof(MappingConfig::stickBehaviorMap[0]);

void MappingConfig::initSD()
{
    if (!SD.begin(CHIPSELECT_PIN))
    {
        Serial.println("MappingConfig: SD Card initialization failed!");
        return;
    }
    Serial.println("MappingConfig: SD Card initialized successfully");
}

bool MappingConfig::loadConfig(const char *filename, JoystickMappingConfig &config)
{
    File file = SD.open(filename, FILE_READ);
    if (!file)
    {
        Serial.print("MappingConfig: Failed to open file: ");
        Serial.println(filename);
        return false;
    }

    Serial.print("MappingConfig: Reading mappings from: ");
    Serial.println(filename);

    JsonDocument doc;

    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error)
    {
        Serial.print("MappingConfig: JSON parsing failed: ");
        Serial.println(error.c_str());
        return false;
    }

    config.setFilename(filename);
    loadMappings(doc, config.mappings, config.numMappings, config.MAX_MAPPINGS);
    loadStickConfig(doc, &config.leftStick, &config.rightStick);

    // Mark config as unmodified since we just loaded it
    config.modified = false;

    return true;
}

bool MappingConfig::saveConfig(const char *filename, JoystickMappingConfig &config)
{
    // Update the filename in config if provided (intentional truncation)
    if (filename != nullptr && filename[0] != '\0')
    {
        config.setFilename(filename);
    }

    // Use the filename from config
    const char *targetFile = config.filename;

    JsonDocument doc;

    saveMappings(doc, config.mappings, config.numMappings);
    saveStickConfig(doc, &config.leftStick, &config.rightStick);

    if (SD.exists(targetFile))
    {
        SD.remove(targetFile);
    }

    File file = SD.open(targetFile, FILE_WRITE);
    if (!file)
    {
        Serial.print("MappingConfig: Failed to create file: ");
        Serial.println(targetFile);
        return false;
    }

    // Write to file
    if (SD.exists(targetFile))
    {
        SD.remove(targetFile);
    }

    file = SD.open(targetFile, FILE_WRITE);
    if (!file)
    {
        Serial.println("MappingConfig: Failed to save stick config");
        return false;
    }
    serializeJsonPretty(doc, file);
    file.close();

    // Mark config as unmodified since we just saved it
    config.modified = false;

    return true;
}

void MappingConfig::loadMappings(JsonDocument &doc, ButtonMapping *mappings, int &numMappings, int maxMappings)
{

    JsonArray mappingsArray = doc["mappings"].as<JsonArray>();
    numMappings = 0;

    for (JsonObject mapping : mappingsArray)
    {
        if (numMappings >= maxMappings)
        {
            Serial.println("MappingConfig: Warning: Too many mappings, truncating");
            break;
        }

        const char *buttonStr = mapping["button"];
        const char *keyStr = mapping["key"];

        int genericButton = JoystickMapping::parseGenericButtonName(buttonStr);
        int keyCode = KeyboardMapping::parseKeyCode(keyStr);

        if (genericButton != -1 && keyCode != -1)
        {
            mappings[numMappings].genericButton = genericButton;
            mappings[numMappings].keyCode = keyCode;
            mappings[numMappings].currentlyPressed = false;

            Serial.print("MappingConfig: Loaded: ");
            Serial.print(buttonStr);
            Serial.print(" -> ");
            Serial.println(keyStr);

            numMappings++;
        }
    }

    Serial.print("MappingConfig: Loaded ");
    Serial.print(numMappings);
    Serial.println(" mappings");
}

void MappingConfig::loadStickConfig(JsonDocument &doc, StickConfig *leftStick, StickConfig *rightStick)
{

    // Load left stick config
    if (doc["leftStick"].is<JsonObject>())
    {
        JsonObject left = doc["leftStick"];
        parseStickConfig(leftStick, left);

        Serial.print("MappingConfig: Left stick: ");
        Serial.println(stickBehaviorToString(leftStick->behavior));
    }

    // Load right stick config
    if (doc["rightStick"].is<JsonObject>())
    {
        JsonObject right = doc["rightStick"];
        parseStickConfig(rightStick, right);

        Serial.print("MappingConfig: Right stick: ");
        Serial.println(stickBehaviorToString(rightStick->behavior));
    }

    Serial.println("MappingConfig: Stick configuration loaded");
}

bool MappingConfig::saveMappings(JsonDocument &doc, ButtonMapping *mappings, int numMappings)
{
    JsonArray mappingsArray = doc["mappings"].to<JsonArray>();

    for (int i = 0; i < numMappings; i++)
    {
        JsonObject mapping = mappingsArray.add<JsonObject>();
        mapping["button"] = JoystickMapping::getGenericButtonName(mappings[i].genericButton);
        mapping["key"] = KeyboardMapping::keyCodeToString(mappings[i].keyCode);
    }

    Serial.print("MappingConfig: Saved ");
    Serial.print(numMappings);
    Serial.println(" mappings");

    return true;
}

bool MappingConfig::saveStickConfig(JsonDocument &doc, StickConfig *leftStick, StickConfig *rightStick)
{
    // Update stick configs
    JsonObject left = doc["leftStick"].to<JsonObject>();
    left["behavior"] = stickBehaviorToString(leftStick->behavior);
    left["sensitivity"] = leftStick->sensitivity;
    left["deadzone"] = leftStick->deadzone;
    left["activationThreshold"] = leftStick->activationThreshold;

    if (leftStick->behavior == StickBehavior::BUTTON_EMULATION)
    {
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

    if (rightStick->behavior == StickBehavior::BUTTON_EMULATION)
    {
        JsonObject rightKeys = right["keys"].to<JsonObject>();
        rightKeys["up"] = KeyboardMapping::keyCodeToString(rightStick->keyUp);
        rightKeys["down"] = KeyboardMapping::keyCodeToString(rightStick->keyDown);
        rightKeys["left"] = KeyboardMapping::keyCodeToString(rightStick->keyLeft);
        rightKeys["right"] = KeyboardMapping::keyCodeToString(rightStick->keyRight);
    }

    Serial.println("MappingConfig: Stick configuration saved");
    return true;
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

StickBehavior MappingConfig::parseStickBehavior(const char *behaviorStr)
{
    for (int i = 0; i < stickBehaviorMapSize; i++)
    {
        if (strcmp(behaviorStr, stickBehaviorMap[i].name) == 0)
        {
            return stickBehaviorMap[i].behavior;
        }
    }

    Serial.print("MappingConfig: Warning: Unknown stick behavior: ");
    Serial.println(behaviorStr);
    return StickBehavior::DISABLED;
}

const char *MappingConfig::stickBehaviorToString(StickBehavior behavior)
{
    for (int i = 0; i < stickBehaviorMapSize; i++)
    {
        if (stickBehaviorMap[i].behavior == behavior)
        {
            return stickBehaviorMap[i].name;
        }
    }

    return "Disabled";
}