#include "mapping/keyboard_mapping.h"
#include <Arduino.h>

int KeyboardMapping::parseKeyCode(const char *keyStr)
{
    // Single characters
    if (strlen(keyStr) == 1)
    {
        return keyStr[0];
    }

    // Special keys
    if (strcmp(keyStr, "RETURN") == 0 || strcmp(keyStr, "ENTER") == 0)
        return KEY_RETURN;
    if (strcmp(keyStr, "ESC") == 0 || strcmp(keyStr, "ESCAPE") == 0)
        return KEY_ESC;
    if (strcmp(keyStr, "BACKSPACE") == 0)
        return KEY_BACKSPACE;
    if (strcmp(keyStr, "TAB") == 0)
        return KEY_TAB;
    if (strcmp(keyStr, "SPACE") == 0)
        return ' ';

    // Modifiers
    if (strcmp(keyStr, "LEFT_CTRL") == 0)
        return KEY_LEFT_CTRL;
    if (strcmp(keyStr, "LEFT_SHIFT") == 0)
        return KEY_LEFT_SHIFT;
    if (strcmp(keyStr, "LEFT_ALT") == 0)
        return KEY_LEFT_ALT;
    if (strcmp(keyStr, "LEFT_GUI") == 0)
        return KEY_LEFT_GUI;
    if (strcmp(keyStr, "RIGHT_CTRL") == 0)
        return KEY_RIGHT_CTRL;
    if (strcmp(keyStr, "RIGHT_SHIFT") == 0)
        return KEY_RIGHT_SHIFT;
    if (strcmp(keyStr, "RIGHT_ALT") == 0)
        return KEY_RIGHT_ALT;
    if (strcmp(keyStr, "RIGHT_GUI") == 0)
        return KEY_RIGHT_GUI;

    // Arrow keys
    if (strcmp(keyStr, "UP") == 0)
        return KEY_UP;
    if (strcmp(keyStr, "DOWN") == 0)
        return KEY_DOWN;
    if (strcmp(keyStr, "LEFT") == 0)
        return KEY_LEFT;
    if (strcmp(keyStr, "RIGHT") == 0)
        return KEY_RIGHT;

    // Function keys
    if (strcmp(keyStr, "F1") == 0)
        return KEY_F1;
    if (strcmp(keyStr, "F2") == 0)
        return KEY_F2;
    if (strcmp(keyStr, "F3") == 0)
        return KEY_F3;
    if (strcmp(keyStr, "F4") == 0)
        return KEY_F4;
    if (strcmp(keyStr, "F5") == 0)
        return KEY_F5;
    if (strcmp(keyStr, "F6") == 0)
        return KEY_F6;
    if (strcmp(keyStr, "F7") == 0)
        return KEY_F7;
    if (strcmp(keyStr, "F8") == 0)
        return KEY_F8;
    if (strcmp(keyStr, "F9") == 0)
        return KEY_F9;
    if (strcmp(keyStr, "F10") == 0)
        return KEY_F10;
    if (strcmp(keyStr, "F11") == 0)
        return KEY_F11;
    if (strcmp(keyStr, "F12") == 0)
        return KEY_F12;

    // Other keys
    if (strcmp(keyStr, "INSERT") == 0)
        return KEY_INSERT;
    if (strcmp(keyStr, "DELETE") == 0)
        return KEY_DELETE;
    if (strcmp(keyStr, "HOME") == 0)
        return KEY_HOME;
    if (strcmp(keyStr, "END") == 0)
        return KEY_END;
    if (strcmp(keyStr, "PAGE_UP") == 0)
        return KEY_PAGE_UP;
    if (strcmp(keyStr, "PAGE_DOWN") == 0)
        return KEY_PAGE_DOWN;

    Serial.print("Warning: Unknown key string: ");
    Serial.println(keyStr);
    return -1;
}

const char *KeyboardMapping::keyCodeToString(int keyCode)
{
    // Special keys
    if (keyCode == KEY_RETURN)
        return "RETURN";
    if (keyCode == KEY_ESC)
        return "ESC";
    if (keyCode == KEY_BACKSPACE)
        return "BACKSPACE";
    if (keyCode == KEY_TAB)
        return "TAB";
    if (keyCode == ' ')
        return "SPACE";

    // Modifiers
    if (keyCode == KEY_LEFT_CTRL)
        return "LEFT_CTRL";
    if (keyCode == KEY_LEFT_SHIFT)
        return "LEFT_SHIFT";
    if (keyCode == KEY_LEFT_ALT)
        return "LEFT_ALT";
    if (keyCode == KEY_LEFT_GUI)
        return "LEFT_GUI";

    // Arrow keys
    if (keyCode == KEY_UP)
        return "UP";
    if (keyCode == KEY_DOWN)
        return "DOWN";
    if (keyCode == KEY_LEFT)
        return "LEFT";
    if (keyCode == KEY_RIGHT)
        return "RIGHT";

    // Single character
    static char buf[2];
    buf[0] = (char)keyCode;
    buf[1] = '\0';
    return buf;
}

int KeyboardMapping::unicodeToKeyCode(int unicode)
{
    // Handle special keys that might come as unicode
    if (unicode == 13 || unicode == 10) // Enter/Return
        return KEY_RETURN;
    if (unicode == 27) // ESC
        return KEY_ESC;
    if (unicode == 8 || unicode == 127) // Backspace/Delete
        return KEY_BACKSPACE;
    if (unicode == 9) // Tab
        return KEY_TAB;
    if (unicode == 32) // Space
        return ' ';

    // Letters: Convert lowercase (97-122) and uppercase (65-90) to KEY_A through KEY_Z
    if (unicode >= 'a' && unicode <= 'z')
    {
        // Lowercase a-z (97-122) -> KEY_A to KEY_Z
        return KEY_A + (unicode - 'a');
    }
    if (unicode >= 'A' && unicode <= 'Z')
    {
        // Uppercase A-Z (65-90) -> KEY_A to KEY_Z
        return KEY_A + (unicode - 'A');
    }

    // Numbers: 0-9 (ASCII 48-57) -> KEY_0 to KEY_9
    if (unicode >= '0' && unicode <= '9')
    {
        return KEY_0 + (unicode - '0');
    }

    // For other printable ASCII characters, return as-is
    // The Teensy keyboard library can handle these directly
    if (unicode >= 32 && unicode < 127)
    {
        return unicode;
    }

    // Unknown/unsupported key
    Serial.print("Warning: Unsupported unicode value: ");
    Serial.println(unicode);
    return unicode; // Return as-is and let the system handle it
}
