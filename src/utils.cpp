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