#include "mapping/keyboard_mapping.h"
#include <Arduino.h>

// Unified key mapping structure
struct KeyMapping
{
    int keyCode;          // KEY_XXX constant
    const char *name;     // Display name (used for string parsing/display)
    const char *altName;  // Alternative name (can be nullptr)
    int unicode;          // Unicode/ASCII value (0 if not applicable)
};

// All non-alphanumeric key mappings
// The unicode field is only used for ASCII characters that come from real keyboard input
// For special keys, the keyboard controller sends KEY_XXX constants directly
static const KeyMapping specialKeyMappings[] = {
    // Special keys (ASCII values for printable chars)
    {KEY_RETURN, "Enter", "Return", 13},      // 0x0D - Carriage Return
    {KEY_ESC, "Esc", "Escape", 27},           // 0x1B - Escape
    {KEY_BACKSPACE, "Backspace", nullptr, 8}, // 0x08 - Backspace
    {KEY_TAB, "Tab", nullptr, 9},             // 0x09 - Tab
    {' ', "Space", nullptr, 32},              // 0x20 - Space

    // Modifiers (no ASCII equivalents)
    {KEY_LEFT_CTRL, "L Ctrl", nullptr, 0},
    {KEY_LEFT_SHIFT, "L Shift", nullptr, 0},
    {KEY_LEFT_ALT, "L Alt", nullptr, 0},
    {KEY_LEFT_GUI, "L Gui", nullptr, 0},
    {KEY_RIGHT_CTRL, "R Ctrl", nullptr, 0},
    {KEY_RIGHT_SHIFT, "R Shift", nullptr, 0},
    {KEY_RIGHT_ALT, "R Alt", nullptr, 0},
    {KEY_RIGHT_GUI, "R Gui", nullptr, 0},

    // Arrow keys (no ASCII equivalents)
    {KEY_UP, "Up", nullptr, 0},
    {KEY_DOWN, "Down", nullptr, 0},
    {KEY_LEFT, "Left", nullptr, 0},
    {KEY_RIGHT, "Right", nullptr, 0},

    // Function keys (no ASCII equivalents)
    {KEY_F1, "F1", nullptr, 0},
    {KEY_F2, "F2", nullptr, 0},
    {KEY_F3, "F3", nullptr, 0},
    {KEY_F4, "F4", nullptr, 0},
    {KEY_F5, "F5", nullptr, 0},
    {KEY_F6, "F6", nullptr, 0},
    {KEY_F7, "F7", nullptr, 0},
    {KEY_F8, "F8", nullptr, 0},
    {KEY_F9, "F9", nullptr, 0},
    {KEY_F10, "F10", nullptr, 0},
    {KEY_F11, "F11", nullptr, 0},
    {KEY_F12, "F12", nullptr, 0},

    // Navigation keys (no ASCII equivalents)
    {KEY_INSERT, "Insert", nullptr, 0},
    {KEY_HOME, "Home", nullptr, 0},
    {KEY_PAGE_UP, "Page Up", nullptr, 0},
    {KEY_DELETE, "Delete", nullptr, 0},
    {KEY_END, "End", nullptr, 0},
    {KEY_PAGE_DOWN, "Page Down", nullptr, 0},

    // Additional special keys (no ASCII equivalents)
    {KEY_PRINTSCREEN, "Prt Sc", "Print Screen", 0},
    {KEY_SCROLL_LOCK, "Scr Lk", "Scroll Lock", 0},
    {KEY_PAUSE, "Pause", "Break", 0},
    {KEY_CAPS_LOCK, "Caps Lock", "Caps Lock", 0},
    {KEY_NUM_LOCK, "Num Lock", "Num Lock", 0},

    // Numpad keys (some have ASCII equivalents)
    {KEYPAD_SLASH, "KP /", "KP_DIVIDE", '/'},
    {KEYPAD_ASTERIX, "KP *", "KP_MULTIPLY", '*'},
    {KEYPAD_MINUS, "KP -", "KP_SUBTRACT", '-'},
    {KEYPAD_PLUS, "KP +", "KP_ADD", '+'},
    {KEYPAD_ENTER, "KP Enter", "KP_ENTER", 0},
    {KEYPAD_1, "KP 1", "KP_1", '1'},
    {KEYPAD_2, "Kp 2", "KP_2", '2'},
    {KEYPAD_3, "KP 3", "KP_3", '3'},
    {KEYPAD_4, "KP 4", "KP_4", '4'},
    {KEYPAD_5, "KP 5", "KP_5", '5'},
    {KEYPAD_6, "KP 6", "KP_6", '6'},
    {KEYPAD_7, "KP 7", "KP_7", '7'},
    {KEYPAD_8, "KP 8", "KP_8", '8'},
    {KEYPAD_9, "KP 9", "KP_9", '9'},
    {KEYPAD_0, "KP 0", "KP_0", '0'},
    {KEYPAD_PERIOD, "KP .", "KP_DOT", '.'},

    // Symbol/Punctuation keys (ASCII values for the unshifted character)
    {KEY_MINUS, "-", "Minus", '-'},           // - and _
    {KEY_EQUAL, "=", "Equals", '='},           // = and +
    {KEY_LEFT_BRACE, "[", "Left Brace", '['},  // [ and {
    {KEY_RIGHT_BRACE, "]", "Right Brace", ']'}, // ] and }
    {KEY_BACKSLASH, "\\", "Backslash", '\\'},  // \ and |
    {KEY_NON_US_NUM, "#", nullptr, 0}, // Non-US # and ~
    {KEY_SEMICOLON, ";", "Semicolon", ';'},    // ; and :
    {KEY_QUOTE, "'", "Quote", '\''},           // ' and "
    {KEY_TILDE, "`", "Tilde", '`'},            // ` and ~
    {KEY_COMMA, ",", "Comma", ','},            // , and <
    {KEY_PERIOD, ".", "Period", '.'},          // . and >
    {KEY_SLASH, "/", "Slash", '/'},            // / and ?

    // USBHost_t36 KeyboardController OEM scan codes (0xC0-0xDF range)
    // These are the actual values sent by the KeyboardController for special keys
    // No bloody clue why these are needed here, but if they arent they dont display properly
    {KEY_F1, "F1_OEM", nullptr, 0xC2},        // F1 reports as 0xC2
    {KEY_F2, "F2_OEM", nullptr, 0xC3},        // F2 reports as 0xC3
    {KEY_F3, "F3_OEM", nullptr, 0xC4},        // F3 reports as 0xC4
    {KEY_F4, "F4_OEM", nullptr, 0xC5},        // F4 reports as 0xC5
    {KEY_F5, "F5_OEM", nullptr, 0xC6},        // F5 reports as 0xC6
    {KEY_F6, "F6_OEM", nullptr, 0xC7},        // F6 reports as 0xC7
    {KEY_F7, "F7_OEM", nullptr, 0xC8},        // F7 reports as 0xC8
    {KEY_F8, "F8_OEM", nullptr, 0xC9},        // F8 reports as 0xC9
    {KEY_F9, "F9_OEM", nullptr, 0xCA},        // F9 reports as 0xCA
    {KEY_F10, "F10_OEM", nullptr, 0xCB},      // F10 reports as 0xCB
    {KEY_F11, "F11_OEM", nullptr, 0xCC},      // F11 reports as 0xCC
    {KEY_F12, "F12_OEM", nullptr, 0xCD},      // F12 reports as 0xCD
    {KEY_INSERT, "INSERT_OEM", nullptr, 0xD1}, // Insert reports as 0xD1
    {KEY_HOME, "HOME_OEM", nullptr, 0xD2},    // Home reports as 0xD2
    {KEY_PAGE_UP, "PAGE_UP_OEM", nullptr, 0xD3}, // Page Up reports as 0xD3
    {KEY_DELETE, "DELETE_OEM", nullptr, 0xD4}, // Delete reports as 0xD4
    {KEY_END, "END_OEM", nullptr, 0xD5},      // End reports as 0xD5
    {KEY_PAGE_DOWN, "PAGE_DOWN_OEM", nullptr, 0xD6}, // Page Down reports as 0xD6
    {KEY_RIGHT, "RIGHT_OEM", nullptr, 0xD7},  // Right Arrow reports as 0xD7
    {KEY_LEFT, "LEFT_OEM", nullptr, 0xD8},    // Left Arrow reports as 0xD8
    {KEY_DOWN, "DOWN_OEM", nullptr, 0xD9},    // Down Arrow reports as 0xD9
    {KEY_UP, "UP_OEM", nullptr, 0xDA},        // Up Arrow reports as 0xDA
};

static const int specialKeyMappingsCount = sizeof(specialKeyMappings) / sizeof(KeyMapping);

int KeyboardMapping::parseKeyCode(const char *keyStr)
{
    // Single characters
    if (strlen(keyStr) == 1)
    {
        return keyStr[0];
    }

    // Check unified key mappings
    for (int i = 0; i < specialKeyMappingsCount; i++)
    {
        if (stricmp(keyStr, specialKeyMappings[i].name) == 0)
        {
            return specialKeyMappings[i].keyCode;
        }
        // Check alternative name if it exists
        if (specialKeyMappings[i].altName != nullptr &&
            stricmp(keyStr, specialKeyMappings[i].altName) == 0)
        {
            return specialKeyMappings[i].keyCode;
        }
    }

    Serial.print("KeyMapping: Warning: Unknown key string: ");
    Serial.println(keyStr);
    return -1;
}

const char *KeyboardMapping::keyCodeToString(int keyCode)
{
    // Check unified key mappings
    for (int i = 0; i < specialKeyMappingsCount; i++)
    {
        if (keyCode == specialKeyMappings[i].keyCode)
        {
            return specialKeyMappings[i].name;
        }
    }

    // Letters: KEY_A through KEY_Z (0xF000 to 0xF019)
    if (keyCode >= KEY_A && keyCode <= KEY_Z)
    {
        static char letterBuf[2];
        letterBuf[0] = 'a' + (keyCode - KEY_A);
        letterBuf[1] = '\0';
        return letterBuf;
    }

    // Numbers: KEY_0 through KEY_9
    if (keyCode >= KEY_0 && keyCode <= KEY_9)
    {
        static char numberBuf[2];
        numberBuf[0] = '0' + (keyCode - KEY_0);
        numberBuf[1] = '\0';
        return numberBuf;
    }

    // Single character (for other ASCII characters that were stored directly)
    if (keyCode >= 32 && keyCode < 127)
    {
        static char buf[2];
        buf[0] = (char)keyCode;
        buf[1] = '\0';
        return buf;
    }

    // Unknown key
    static char unknownBuf[8];
    snprintf(unknownBuf, sizeof(unknownBuf), "0x%X", keyCode);
    return unknownBuf;
}

int KeyboardMapping::unicodeToKeyCode(int unicode)
{
    // First check if the unicode value is already a KEY_XXX constant
    // The keyboard controller may send KEY constants directly (e.g., KEY_INSERT = 0xD1)
    // If it matches a keyCode in our mapping, it's already the correct value
    for (int i = 0; i < specialKeyMappingsCount; i++)
    {
        if (unicode == specialKeyMappings[i].keyCode)
        {
            return unicode; // Already a KEY constant, return as-is
        }
    }

    // Check unified key mappings for HID/unicode values
    for (int i = 0; i < specialKeyMappingsCount; i++)
    {
        if (specialKeyMappings[i].unicode != 0 && unicode == specialKeyMappings[i].unicode)
        {
            return specialKeyMappings[i].keyCode;
        }
    }

    // Special case: handle line feed (10) as return/enter
    if (unicode == 10)
    {
        return KEY_RETURN;
    }

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
    Serial.print("KeyMapping: Warning: Unsupported unicode value: 0x");
    Serial.print(unicode, HEX);
    Serial.print(" (");
    Serial.print(unicode);
    Serial.println(")");
    return unicode; // Return as-is and let the system handle it
}
