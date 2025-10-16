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

    // Extract filename without path or extension into buffer (no heap allocation!)
    // Input: "/path/to/file.json" or "file.json"
    // Output buffer: "file"
    static void trimFilenameToBuffer(const char *fullPath, char *outputBuffer, int bufferSize);
};

#endif // UTILS_H