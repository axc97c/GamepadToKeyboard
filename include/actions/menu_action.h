#ifndef MENU_ACTION_H
#define MENU_ACTION_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <USBHost_t36.h>
#include "actions/action.h"

struct MenuItem
{
    String name;
    String identifier;
    uint32_t data;

    MenuItem() : name(""), identifier(""), data(0) {}
    MenuItem(String n, String id, uint32_t d = 0) : name(n), identifier(id), data(d) {}
};

class MenuAction : public Action
{
protected:
    MenuActionParams params;

    static const int MAX_ITEMS = 32; // Match JoystickMappingConfig::MAX_MAPPINGS

    String menuTitle;
    MenuItem menuItems[MAX_ITEMS];
    int menuItemCount;

    int selectedIndex;
    int scrollOffset;

    // Timeout tracking
    unsigned long lastInputTime;
    static const unsigned long TIMEOUT_MS = 30000;

    // Protected methods for derived classes
    void moveUp();
    void moveDown();
    void updateScrollOffset();
    void displayMenu();
    void resetTimeout();
    bool checkTimeout();

public:
    MenuAction(DeviceManager *dev, ActionHandler *hdlr, MenuActionParams p);
    virtual ~MenuAction() {}

    // Action lifecycle methods
    void init() override;
    void loop() override;
    ActionType getType() override;

    // Pure virtual methods that derived classes must implement
    virtual void onInit() = 0;
    virtual void onConfirm() = 0;
    virtual void onCancel() = 0;

    // Public methods to configure menu
    void setMenu(String title, MenuItem items[], int itemCount);

    // Get current selection
    int getSelectedIndex();
    MenuItem getSelectedItem();

    // Force a menu refresh
    void refresh();
};

#endif // MENU_ACTION_H