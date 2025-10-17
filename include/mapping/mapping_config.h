#ifndef MAPPING_CONFIG_H
#define MAPPING_CONFIG_H

#include <Arduino.h>
#include <SD.h>
#include <ArduinoJson.h>
#include "actions/action_types.h"

class MappingConfig
{
private:
    static const int CHIPSELECT_PIN = BUILTIN_SDCARD; // Teensy 4.1 built-in SD

    // Stick behavior string mapping
    struct StickBehaviorMapping
    {
        StickBehavior behavior;
        const char *name;
    };
    
    static const StickBehaviorMapping stickBehaviorMap[];
    static const int stickBehaviorMapSize;   
    
    struct TriggerBehaviorMapping
    {
        TriggerBehavior behavior;
        const char *name;
    };
    
    static const TriggerBehaviorMapping triggerBehaviorMap[];
    static const int triggerBehaviorMapSize;

public:
    static bool loadConfig(const char *filename, JoystickMappingConfig &config);
    static bool saveConfig(const char *filename, JoystickMappingConfig &config);

    static void initSD();

    // Make these public for Utils class
    static StickBehavior parseStickBehavior(const char *behaviorStr);
    static const char *stickBehaviorToString(StickBehavior behavior);

    static TriggerBehavior parseTriggerBehavior(const char *behaviorStr);
    static const char *triggerBehaviorToString(TriggerBehavior behaviour);

private:
    static void loadMappings(JsonDocument &doc, ButtonMapping *mappings, int &numMappings, int maxMappings);
    static void loadStickConfig(JsonDocument &doc, StickConfig *leftStick, StickConfig *rightStick);
    static void loadTriggerConfig(JsonDocument &doc, TriggerConfig *trigger);

    static bool saveMappings(JsonDocument &doc, ButtonMapping *mappings, int numMappings);
    static bool saveStickConfig(JsonDocument &doc, StickConfig *leftStick, StickConfig *rightStick);
    static bool saveTriggerConfig(JsonDocument &doc, TriggerConfig *trigger);

    static void parseStickConfig(StickConfig *leftStick, ArduinoJson::V742PB22::JsonObject &left);
};

#endif