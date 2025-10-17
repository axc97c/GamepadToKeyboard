#ifndef MENU_ACTION_H
#define MENU_ACTION_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <USBHost_t36.h>
#include "actions/action.h"
#include "actions/action_handler.h"

struct MenuItem
{
    static const int MAX_NAME_LEN = 20;
    static const int MAX_ID_LEN = 16;

    char name[MAX_NAME_LEN];
    char identifier[MAX_ID_LEN];
    uint32_t data;
    bool used; // Flag to mark if this slot is in use

    MenuItem() : data(0), used(false)
    {
        name[0] = '\0';
        identifier[0] = '\0';
    }

    MenuItem(const char *n, const char *id, uint32_t d = 0) : data(d), used(true)
    {
        setName(n);
        setIdentifier(id);
    }

    void set(const char *n, const char *id, uint32_t d = 0)
    {
        setName(n);
        setIdentifier(id);
        data = d;
        used = true;
    }

    void clear()
    {
        name[0] = '\0';
        identifier[0] = '\0';
        data = 0;
        used = false;
    }

    void setName(const char *n)
    {
        if (n != nullptr)
        {
// Intentional truncation with proper null termination
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
            strncpy(name, n, MAX_NAME_LEN - 1);
#pragma GCC diagnostic pop
            name[MAX_NAME_LEN - 1] = '\0';
        }
        else
        {
            name[0] = '\0';
        }
    }

    void setIdentifier(const char *id)
    {
        if (id != nullptr)
        {
// Intentional truncation with proper null termination
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
            strncpy(identifier, id, MAX_ID_LEN - 1);
#pragma GCC diagnostic pop
            identifier[MAX_ID_LEN - 1] = '\0';
        }
        else
        {
            identifier[0] = '\0';
        }
    }
};

class MenuAction : public Action
{
protected:
    static const int MAX_ITEMS = 32; // Match JoystickMappingConfig::MAX_MAPPINGS
    static const int MAX_TITLE_LEN = 20;

    char menuTitle[MAX_TITLE_LEN];
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
    MenuAction(DeviceManager *dev, ActionHandler *hdlr);
    virtual ~MenuAction() {}

    // Action lifecycle methods
    void init() override;
    void loop() override;
    ActionType getType() override;

    // Pure virtual methods that derived classes must implement
    virtual void onInit() {};
    virtual void onConfirm() = 0;
    virtual void onCancel() { handler->popAction(); };

    // Public methods to configure menu
    void setMenu(const char *title, MenuItem items[], int itemCount);
    void setTitle(const char *title); // Set title only

    // Get current selection
    int getSelectedIndex();
    MenuItem getSelectedItem();

    // Force a menu refresh
    void refresh();

    virtual void clear();
    void addItem(const char *name, const char *id, uint32_t data = 0)
    {
        menuItems[menuItemCount++].set(name, id, data);
    }
};

#endif // MENU_ACTION_H