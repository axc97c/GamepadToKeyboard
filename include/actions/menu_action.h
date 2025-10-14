#ifndef MENU_ACTION_H
#define MENU_ACTION_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <USBHost_t36.h>
#include "actions/action.h"

class MenuAction : public Action
{
private:
    MenuActionParams params;

    static const int MAX_ITEMS = 20;

    String menuTitle;
    String menuItems[MAX_ITEMS];
    int menuItemCount;

    int selectedIndex;
    int scrollOffset;

    void moveUp();
    void moveDown();
    void updateScrollOffset();
    void displayMenu();

public:
    MenuAction(DeviceManager *dev, ActionHandler *hdlr, MenuActionParams p);

    // Action lifecycle methods
    void init() override;
    void loop() override;
    ActionType getType() override;

    // Public methods to configure menu
    void setMenu(String title, String items[], int itemCount);

    // Get current selection
    int getSelectedIndex();
    String getSelectedItem();

    // Force a menu refresh
    void refresh();
};

#endif // MENU_ACTION_H