#ifndef KEYBOARD_MAPPINGS_H
#define KEYBOARD_MAPPINGS_H

class KeyboardMapping {
  public:
    static int parseKeyCode(const char* keyStr);
    static const char* keyCodeToString(int keyCode);
};


#endif