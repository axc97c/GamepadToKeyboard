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
        Serial.println("Failed to open root directory");
        String errorItem[] = {"No SD card found"};
        setMenu(menuTitle, errorItem, 1);
        return;
    }
    
    if (!root.isDirectory())
    {
        Serial.println("Root is not a directory");
        root.close();
        String errorItem[] = {"SD card error"};
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
                
                Serial.print("Found config file: ");
                Serial.println(filename);
            }
        }
        entry.close();
        entry = root.openNextFile();
    }
    root.close();
    
    if (fileCount == 0)
    {
        Serial.println("No JSON config files found");
        String errorItem[] = {"No configs found"};
        setMenu(menuTitle, errorItem, 1);
        return;
    }
    
    // Sort the files alphabetically
    sortConfigFiles(fileCount);
    
    // Create display names (without path or extension) for menu
    String displayNames[MAX_CONFIG_FILES];
    for (int i = 0; i < fileCount; i++)
    {
        displayNames[i] = Utils::trimFilename(configFiles[i]);
        
        Serial.print("Display name: ");
        Serial.print(displayNames[i]);
        Serial.print(" -> ");
        Serial.println(configFiles[i]);
    }
    
    
    // Set up the menu with display names
    setMenu(menuTitle, displayNames, fileCount);
    
    Serial.print("Loaded ");
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
    int selectedIdx = getSelectedIndex();
    String selectedItem = getSelectedItem();
    
    Serial.print("Load config - Item confirmed: ");
    Serial.print(selectedItem);
    Serial.print(" (index: ");
    Serial.print(selectedIdx);
    Serial.println(")");
    
    // Check if there are no configs or error state
    if (selectedItem == "No SD card found" || 
        selectedItem == "SD card error" || 
        selectedItem == "No configs found")
    {
        Serial.println("Cannot load config - error state");
        return;
    }
    
    // Get the full filename from the stored array
    if (selectedIdx >= 0 && selectedIdx < MAX_CONFIG_FILES)
    {
        String fullFilename = configFiles[selectedIdx];
        
        Serial.print("Loading config file: ");
        Serial.println(fullFilename);
        
        // Convert String to const char* for RunActionParams
        static char filenameBuffer[64];
        fullFilename.toCharArray(filenameBuffer, sizeof(filenameBuffer));
        
        // Activate run action with selected config file
        handler->activateRun({filenameBuffer});
    }
}

void LoadConfigMenuAction::onCancel()
{
    // Handle cancel - return to main menu
    Serial.println("Load config menu - Cancel/Back pressed, returning to main menu");
    
    handler->activateMainMenu({0});
}