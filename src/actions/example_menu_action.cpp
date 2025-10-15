#include "actions/example_menu_action.h"
#include "devices.h"

ExampleMenuAction::ExampleMenuAction(DeviceManager *dev, ActionHandler *hdlr, MenuActionParams p)
    : MenuAction(dev, hdlr, p)
{
}

void ExampleMenuAction::onInit()
{
    // Initialize the example menu
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

    Serial.println("ExampleMenuAction setup complete");
}

void ExampleMenuAction::onConfirm()
{
    // Handle confirmation for the example menu
    String selectedItem = getSelectedItem();
    int selectedIdx = getSelectedIndex();
    
    Serial.print("Example menu - Item confirmed: ");
    Serial.print(selectedItem);
    Serial.print(" (index: ");
    Serial.print(selectedIdx);
    Serial.println(")");
    
    // Add specific handling for each menu item
    // For now, just print what was selected
    // In a real implementation, you might:
    // - Switch to a different action
    // - Load a profile
    // - Enter a submenu
    // - etc.
}

void ExampleMenuAction::onCancel()
{
    // Handle cancel for the example menu
    Serial.println("Example menu - Cancel/Back pressed");
    
    // In a real implementation, you might:
    // - Return to a previous menu
    // - Switch to run mode
    // - etc.
}