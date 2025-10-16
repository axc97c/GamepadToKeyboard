#include "actions/load_config_menu_action.h"
#include "actions/action_handler.h"
#include "devices.h"
#include "utils.h"
#include <SD.h>

LoadConfigMenuAction::LoadConfigMenuAction(DeviceManager *dev, ActionHandler *hdlr, MenuActionParams p)
    : MenuAction(dev, hdlr, p)
{
    // Set the fixed title in constructor since it never changes
    setTitle("Configs");
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
    // Clear existing menu items first
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        menuItems[i].clear();
    }
    menuItemCount = 0;

    // Title already set in constructor
    int fileCount = 0;

    // Open root directory
    File root = SD.open("/");
    if (!root)
    {
        Serial.println("LoadConfigMenuAction: Failed to open root directory");
        // Reuse pre-allocated item instead of temp array
        menuItems[0].set("No SD card found", "error", 0);
        menuItemCount = 1;
        return;
    }

    if (!root.isDirectory())
    {
        Serial.println("LoadConfigMenuAction: Root is not a directory");
        root.close();
        // Reuse pre-allocated item instead of temp array
        menuItems[0].set("SD card error", "error", 0);
        menuItemCount = 1;
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
        // Reuse pre-allocated item instead of temp array
        menuItems[0].set("No configs found", "error", 0);
        menuItemCount = 1;
        return;
    }

    // Sort the files alphabetically
    sortConfigFiles(fileCount);

    // Populate pre-allocated menu items directly (no temp array allocation!)
    for (int i = 0; i < fileCount; i++)
    {
        String displayName = Utils::trimFilename(configFiles[i]);

        // Convert Strings to char arrays for set() method
        char nameBuffer[MenuItem::MAX_NAME_LEN];
        char idBuffer[MenuItem::MAX_ID_LEN];

        displayName.toCharArray(nameBuffer, sizeof(nameBuffer));
        configFiles[i].toCharArray(idBuffer, sizeof(idBuffer));

        menuItems[i].set(nameBuffer, idBuffer, i);

        Serial.print("LoadConfigMenuAction: Display name: ");
        Serial.print(displayName);
        Serial.print(" -> ");
        Serial.println(configFiles[i]);
    }

    menuItemCount = fileCount;

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

    // Check if there are no configs or error state (identifier is now char array)
    if (strcmp(selectedItem.identifier, "error") == 0)
    {
        Serial.println("LoadConfigMenuAction: Cannot load config - error state");
        return;
    }

    // Use the identifier which contains the full filename path
    const char* fullFilename = selectedItem.identifier;

    Serial.print("LoadConfigMenuAction: Loading config file: ");
    Serial.println(fullFilename);

    // fullFilename is already a const char* from identifier, pass directly
    handler->activateRun({fullFilename});
}

void LoadConfigMenuAction::onCancel()
{
    // Handle cancel - return to previous action
    Serial.println("LoadConfigMenuAction: Cancel/Back pressed, returning to previous action");

    handler->popAction();
}