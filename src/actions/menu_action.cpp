#include "actions/menu_action.h"
#include "devices.h"

MenuAction::MenuAction(DeviceManager *dev, ActionHandler *hdlr, MenuActionParams p)
    : Action(dev, hdlr),
      params(p)
{
    selectedIndex = 0;
    scrollOffset = 0;
    menuItemCount = 0;
}

void MenuAction::init()
{
    // Initialize default menu
    menuTitle = "Main Menu";
    String defaultItems[] = {
        "Option 1",
        "Option 2",
        "Option 3",
        "Option 4",
        "Option 5",
        "Option 6",
        "Option 7",
        "Option 8",
        "Option 9",
        "Option 10"};
        
    setMenu(menuTitle, defaultItems, 10);

    Serial.println("MenuAction setup complete");

    selectedIndex = 0;
    scrollOffset = 0;

    displayMenu();

    Serial.println("MenuAction initialized");
    Serial.print("Selected: ");
    Serial.println(menuItems[selectedIndex]);
}

void MenuAction::loop()
{
    // Get input event from the input handler
    InputEvent event = devices->getInput()->getEvent();

    // Handle the event
    switch (event)
    {
    case INPUT_UP:
        moveUp();
        break;

    case INPUT_DOWN:
        moveDown();
        break;

    case INPUT_CONFIRM:
        // Handle selection confirmation
        Serial.print("Confirmed: ");
        Serial.println(menuItems[selectedIndex]);
        // TODO: Add callback or action handler here
        break;

    case INPUT_CANCEL:
        Serial.println("Cancel pressed");
        // TODO: Add back/cancel action here
        break;

    case INPUT_NONE:
    default:
        // No input, do nothing
        break;
    }
}

void MenuAction::moveUp()
{
    if (selectedIndex > 0)
    {
        selectedIndex--;
        updateScrollOffset();
        displayMenu();
        Serial.print("Selected: ");
        Serial.println(menuItems[selectedIndex]);
    }
}

void MenuAction::moveDown()
{
    if (selectedIndex < menuItemCount - 1)
    {
        selectedIndex++;
        updateScrollOffset();
        displayMenu();
        Serial.print("Selected: ");
        Serial.println(menuItems[selectedIndex]);
    }
}

void MenuAction::updateScrollOffset()
{
    // Keep selected item in the middle (row 2, which is index 2)
    // Row 0: Title
    // Row 1: Item above selected
    // Row 2: Selected item (with >)
    // Row 3: Item below selected

    // The selected item should appear on row 2 (third row)
    scrollOffset = selectedIndex - 1;

    // Adjust for top boundary
    if (scrollOffset < 0)
    {
        scrollOffset = 0;
    }

    // Adjust for bottom boundary
    if (scrollOffset > menuItemCount - 3)
    {
        scrollOffset = max(0, menuItemCount - 3);
    }
}

void MenuAction::displayMenu()
{
    LiquidCrystal_I2C *lcd = devices->getLCD();

    lcd->clear();

    // Row 0: Display title
    lcd->setCursor(0, 0);
    lcd->print(menuTitle);

    // Rows 1-3: Display menu items (3 visible items)
    for (int i = 0; i < 3; i++)
    {
        int itemIndex = scrollOffset + i;

        if (itemIndex < menuItemCount)
        {
            lcd->setCursor(0, i + 1);

            // Add > indicator for selected item
            if (itemIndex == selectedIndex)
            {
                lcd->print(">");
                lcd->print(menuItems[itemIndex]);
            }
            else
            {
                lcd->print(" ");
                lcd->print(menuItems[itemIndex]);
            }
        }
    }
}

void MenuAction::setMenu(String title, String items[], int itemCount)
{
    menuTitle = title;
    menuItemCount = min(itemCount, MAX_ITEMS);
    for (int i = 0; i < menuItemCount; i++)
    {
        menuItems[i] = items[i];
    }
    selectedIndex = 0;
    scrollOffset = 0;
}

int MenuAction::getSelectedIndex()
{
    return selectedIndex;
}

String MenuAction::getSelectedItem()
{
    return menuItems[selectedIndex];
}

void MenuAction::refresh()
{
    displayMenu();
}

ActionType MenuAction::getType()
{
    return ActionType::MENU;
}
