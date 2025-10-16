#ifndef MEMORY_H
#define MEMORY_H

#include <Arduino.h>

class MemoryMonitor
{
public:
    // Initialize memory monitoring (call once in setup)
    static void init();

    // Call this in your main loop - checks if it's time to report and prints if so
    static void update();

    // Print memory usage report immediately (can call manually)
    static void printMemoryUsage();

    // Set the reporting interval (default 10 seconds)
    static void setReportInterval(unsigned long intervalMs);

private:
    static unsigned long lastMemoryReport;
    static unsigned long memoryReportInterval;
};

#endif // MEMORY_H
