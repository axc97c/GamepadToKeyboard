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
    static bool loadMappings(const char* filename, ButtonMapping* mappings, int& numMappings, int maxMappings);
    static bool saveMappings(const char* filename, ButtonMapping* mappings, int numMappings);
    static void initSD();
    
  private:
    static int parseKeyCode(const char* keyStr);
    static const char* keyCodeToString(int keyCode);
    static int parseButtonName(const char* buttonStr);  // Add this
    static const char* buttonNameToString(uint8_t buttonNum);  // Add this
};

#endif