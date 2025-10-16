#include "actions/load_config_menu_action.h"
#include "actions/action_handler.h"
#include "devices.h"
#include "utils.h"
#include <SD.h>

LoadConfigMenuAction::LoadConfigMenuAction(DeviceManager *dev, ActionHandler *hdlr, MenuActionParams p)
    : MenuAction(dev, hdlr, p)
{
}

void LoadConfigMenuAction::onInit()
{
    Serial.println("LoadConfigMenuAction: Scanning for config files...");
    
    // Scan for JSON files on SD card
    scanConfigFiles();
    
    Serial.println("LoadConfigMenuAction setup complete");
}

void LoadConfigMenuAction::scanConfigFiles()
{
    menuTitle = "Configs";
    int fileCount = 0;
    
    // Open root directory
    File root = SD.open("/");
    if (!root)
    {
        Serial.println("LoadConfigMenuAction: Failed to open root directory");
        MenuItem errorItem[] = {MenuItem("No SD card found", "error", 0)};
        setMenu(menuTitle, errorItem, 1);
        return;
    }

    if (!root.isDirectory())
    {
        Serial.println("LoadConfigMenuAction: Root is not a directory");
        root.close();
        MenuItem errorItem[] = {MenuItem("SD card error", "error", 0)};
        setMenu(menuTitle, errorItem, 1);
        return;
    }
    
    // Read all files in root directory
    File entry = root.openNextFile();
    while (entry && fileCount < MAX_CONFIG_FILES)
    {
        if (!entry.isDirectory())
        {
            String filename = entry.name();
            
            // Check if it's a JSON file
            if (filename.endsWith(".json") || filename.endsWith(".JSON"))
            {
                // Store full filename with path
                configFiles[fileCount] = "/" + filename;
                fileCount++;
                
                Serial.print("LoadConfigMenuAction: Found config file: ");
                Serial.println(filename);
            }
        }
        entry.close();
        entry = root.openNextFile();
    }
    root.close();
    
    if (fileCount == 0)
    {
        Serial.println("LoadConfigMenuAction: No JSON config files found");
        MenuItem errorItem[] = {MenuItem("No configs found", "error", 0)};
        setMenu(menuTitle, errorItem, 1);
        return;
    }

    // Sort the files alphabetically
    sortConfigFiles(fileCount);

    // Create menu items (with display names and full path as identifier)
    MenuItem menuItems[MAX_CONFIG_FILES];
    for (int i = 0; i < fileCount; i++)
    {
        String displayName = Utils::trimFilename(configFiles[i]);
        menuItems[i] = MenuItem(displayName, configFiles[i], i);

        Serial.print("LoadConfigMenuAction: Display name: ");
        Serial.print(displayName);
        Serial.print(" -> ");
        Serial.println(configFiles[i]);
    }


    // Set up the menu with menu items
    setMenu(menuTitle, menuItems, fileCount);
    
    Serial.print("LoadConfigMenuAction: Loaded ");
    Serial.print(fileCount);
    Serial.println(" config files");
}

void LoadConfigMenuAction::sortConfigFiles(int count)
{
    // Simple bubble sort for alphabetical ordering
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = 0; j < count - i - 1; j++)
        {
            // Extract filenames without path for comparison
            String name1 = configFiles[j].substring(1); // Remove leading '/'
            String name2 = configFiles[j + 1].substring(1);
            
            // Compare case-insensitive
            name1.toLowerCase();
            name2.toLowerCase();
            
            if (name1.compareTo(name2) > 0)
            {
                // Swap
                String temp = configFiles[j];
                configFiles[j] = configFiles[j + 1];
                configFiles[j + 1] = temp;
            }
        }
    }
}

void LoadConfigMenuAction::onConfirm()
{
    MenuItem selectedItem = getSelectedItem();

    Serial.print("LoadConfigMenuAction: Load config - Item confirmed: ");
    Serial.print(selectedItem.name);
    Serial.print(" (identifier: ");
    Serial.print(selectedItem.identifier);
    Serial.print(", data: ");
    Serial.print(selectedItem.data);
    Serial.println(")");

    // Check if there are no configs or error state
    if (selectedItem.identifier == "error")
    {
        Serial.println("LoadConfigMenuAction: Cannot load config - error state");
        return;
    }

    // Use the identifier which contains the full filename path
    String fullFilename = selectedItem.identifier;

    Serial.print("LoadConfigMenuAction: Loading config file: ");
    Serial.println(fullFilename);

    // Convert String to const char* for RunActionParams
    static char filenameBuffer[64];
    fullFilename.toCharArray(filenameBuffer, sizeof(filenameBuffer));

    // Activate run action with selected config file
    handler->activateRun({filenameBuffer});
}

void LoadConfigMenuAction::onCancel()
{
    // Handle cancel - return to previous action
    Serial.println("LoadConfigMenuAction: Cancel/Back pressed, returning to previous action");

    handler->popAction();
}