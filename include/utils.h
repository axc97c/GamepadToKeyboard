#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

class Utils
{
public:
    // Extract filename without path or extension
    // Input: "/path/to/file.json" or "file.json"
    // Output: "file"
    static String trimFilename(const String &fullPath);
    
    // Extract filename without path or extension from char*
    static String trimFilename(const char *fullPath);
};

#endif // UTILS_H