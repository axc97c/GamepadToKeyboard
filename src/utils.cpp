#include "utils.h"

String Utils::trimFilename(const String &fullPath)
{
    int lastSlash = fullPath.lastIndexOf('/');
    int lastDot = fullPath.lastIndexOf('.');
    
    String displayName;
    
    // Remove path
    if (lastSlash >= 0)
    {
        displayName = fullPath.substring(lastSlash + 1);
    }
    else
    {
        displayName = fullPath;
    }
    
    // Remove extension (only if the dot comes after the last slash)
    if (lastDot > lastSlash)
    {
        displayName = displayName.substring(0, lastDot - (lastSlash + 1));
    }
    
    return displayName;
}

String Utils::trimFilename(const char *fullPath)
{
    return trimFilename(String(fullPath));
}

void Utils::trimFilenameToBuffer(const char *fullPath, char *outputBuffer, int bufferSize)
{
    if (fullPath == nullptr || outputBuffer == nullptr || bufferSize <= 0)
    {
        if (outputBuffer != nullptr && bufferSize > 0)
        {
            outputBuffer[0] = '\0';
        }
        return;
    }

    // Find last slash and last dot
    const char *lastSlash = strrchr(fullPath, '/');
    const char *lastDot = strrchr(fullPath, '.');

    // Start pointer (after slash or at beginning)
    const char *start = (lastSlash != nullptr) ? (lastSlash + 1) : fullPath;

    // End pointer (at dot or at string end)
    const char *end = (lastDot != nullptr && lastDot > start) ? lastDot : (fullPath + strlen(fullPath));

    // Calculate length
    int len = end - start;
    if (len >= bufferSize)
    {
        len = bufferSize - 1; // Leave room for null terminator
    }

    // Copy the substring (intentional truncation with null termination)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wstringop-truncation"
    strncpy(outputBuffer, start, len);
    #pragma GCC diagnostic pop
    outputBuffer[len] = '\0';
}