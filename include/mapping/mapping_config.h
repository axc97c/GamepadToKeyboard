#ifndef MAPPING_CONFIG_H
#define MAPPING_CONFIG_H

#include <Arduino.h>
#include <SD.h>
#include <ArduinoJson.h>
#include "actions/action_types.h"

class MappingConfig {
  private:
    static const int CHIPSELECT_PIN = BUILTIN_SDCARD;  // Teensy 4.1 built-in SD
    
  public:
    static bool loadConfig(const char* filename, ButtonMapping* mappings, int& numMappings, int maxMappings, StickConfig* leftStick, StickConfig* rightStick);
    static bool saveConfig(const char* filename, ButtonMapping* mappings, int& numMappings, StickConfig* leftStick, StickConfig* rightStick);

    static void initSD();
    
  private:
    static StickBehavior parseStickBehavior(const char* behaviorStr);
    static const char* stickBehaviorToString(StickBehavior behavior);

    static void loadMappings(JsonDocument& doc, ButtonMapping* mappings, int& numMappings, int maxMappings);
    static void loadStickConfig(JsonDocument& doc, StickConfig *leftStick, StickConfig *rightStick);

    static bool saveMappings(JsonDocument& doc, ButtonMapping* mappings, int numMappings); 
    static bool saveStickConfig(JsonDocument& doc, StickConfig* leftStick, StickConfig* rightStick);
    
    static void parseStickConfig(StickConfig *leftStick, ArduinoJson::V742PB22::JsonObject &left);
};

#endif