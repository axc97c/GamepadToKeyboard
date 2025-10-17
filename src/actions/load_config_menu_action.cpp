#include "actions/load_config_menu_action.h"
#include "actions/action_handler.h"
#include "devices.h"
#include "utils.h"
#include <SD.h>

namespace {
    constexpr const char* MENU_ERROR = "error";
}

LoadConfigMenuAction::LoadConfigMenuAction(DeviceManager *dev, ActionHandler *hdlr)
    : MenuAction(dev, hdlr)
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
    clear();

    // Title already set in constructor
    int fileCount = 0;

    // Open root directory
    File root = SD.open("/");
    if (!root)
    {
        Serial.println("LoadConfigMenuAction: Failed to open root directory");
        // Reuse pre-allocated item instead of temp array
        addItem("No SD card found", MENU_ERROR, 0);
        return;
    }

    if (!root.isDirectory())
    {
        Serial.println("LoadConfigMenuAction: Root is not a directory");
        root.close();
        // Reuse pre-allocated item instead of temp array
        addItem("SD card error", MENU_ERROR, 0);
        return;
    }

    // Read all files in root directory
    File entry = root.openNextFile();
    while (entry && fileCount < MAX_CONFIG_FILES)
    {
        if (!entry.isDirectory())
        {
            const char *filename = entry.name();
            int len = strlen(filename);

            // Check if it's a JSON file (case insensitive)
            if (len > 5 &&
                (strcmp(filename + len - 5, ".json") == 0 ||
                 strcmp(filename + len - 5, ".JSON") == 0))
            {
                // Store full filename with path (no heap allocation!)
                snprintf(configFiles[fileCount], MAX_FILENAME_LEN, "/%s", filename);

                Serial.print("LoadConfigMenuAction: Found config file: ");
                Serial.println(configFiles[fileCount]);

                fileCount++;
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
        addItem("No configs found", MENU_ERROR, 0);
        return;
    }

    // Sort the files alphabetically
    sortConfigFiles(fileCount);

    // Populate pre-allocated menu items directly (no temp array allocation!)
    for (int i = 0; i < fileCount; i++)
    {
        char displayName[MenuItem::MAX_NAME_LEN];

        // Extract display name without path/extension (no heap allocation!)
        Utils::trimFilenameToBuffer(configFiles[i], displayName, sizeof(displayName));

        // Use full path as identifier (already in char array)
        addItem(displayName, configFiles[i], i);

        Serial.print("LoadConfigMenuAction: Display name: ");
        Serial.print(displayName);
        Serial.print(" -> ");
        Serial.println(configFiles[i]);
    }

    Serial.print("LoadConfigMenuAction: Loaded ");
    Serial.print(fileCount);
    Serial.println(" config files");
}

void LoadConfigMenuAction::sortConfigFiles(int count)
{
    // Simple bubble sort for alphabetical ordering (no heap allocation!)
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = 0; j < count - i - 1; j++)
        {
            // Compare filenames case-insensitive (skip leading '/' if present)
            const char *name1 = configFiles[j][0] == '/' ? &configFiles[j][1] : configFiles[j];
            const char *name2 = configFiles[j + 1][0] == '/' ? &configFiles[j + 1][1] : configFiles[j + 1];

            // Case-insensitive comparison
            if (strcasecmp(name1, name2) > 0)
            {
                // Swap using char array swap (no heap allocation!)
                // Intentional full buffer copy (already null-terminated strings)
                char temp[MAX_FILENAME_LEN];
                strncpy(temp, configFiles[j], MAX_FILENAME_LEN);
                strncpy(configFiles[j], configFiles[j + 1], MAX_FILENAME_LEN);
                strncpy(configFiles[j + 1], temp, MAX_FILENAME_LEN);
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
    if (strcmp(selectedItem.identifier, MENU_ERROR) == 0)
    {
        Serial.println("LoadConfigMenuAction: Cannot load config - error state");
        return;
    }

    // fullFilename is already a const char* from identifier, pass directly

    RunActionParams runParams;
    strncpy(runParams.filename, selectedItem.identifier, MAX_FILENAME_LEN);
    Serial.print("LoadConfigMenuAction: Loading config file: ");
    Serial.println(runParams.filename);

    handler->activateRun(runParams);
}

void LoadConfigMenuAction::clear()
{
    for (int i = 0; i < MAX_CONFIG_FILES; i++)
    {
        configFiles[i][0] = '\0';
    }

    MenuAction::clear();
}
