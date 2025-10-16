#include "memory.h"

// Static member initialization
unsigned long MemoryMonitor::lastMemoryReport = 0;
unsigned long MemoryMonitor::memoryReportInterval = 60000;

void MemoryMonitor::init()
{
    lastMemoryReport = millis();
    Serial.println("MemoryMonitor: Initialized (reporting every 10 seconds)");
}

void MemoryMonitor::update()
{
    unsigned long currentTime = millis();
    if (currentTime - lastMemoryReport >= memoryReportInterval)
    {
        lastMemoryReport = currentTime;
        printMemoryUsage();
    }
}

void MemoryMonitor::setReportInterval(unsigned long intervalMs)
{
    memoryReportInterval = intervalMs;
    Serial.print("MemoryMonitor: Report interval set to ");
    Serial.print(intervalMs);
    Serial.println(" ms");
}

void MemoryMonitor::printMemoryUsage()
{
    // Teensy 4.1 memory layout:
    // [Low Memory]
    //   .data segment (initialized global/static variables)
    //   .bss segment (uninitialized global/static variables - zero-initialized)
    //   heap (grows upward ->)
    //   ... free space ...
    //   stack (grows downward <-)
    // [High Memory]

    // Get linker symbols for memory regions
    extern unsigned long _sdata;     // Start of .data
    extern unsigned long _edata;     // End of .data
    extern unsigned long _sbss;      // Start of .bss
    extern unsigned long _ebss;      // End of .bss
    extern unsigned long _heap_start;
    extern unsigned long _heap_end;
    extern unsigned long _estack;    // End of stack (top of RAM)
    extern char *__brkval;           // Current heap top

    // Calculate memory region sizes
    unsigned long dataSize = (unsigned long)&_edata - (unsigned long)&_sdata;
    unsigned long bssSize = (unsigned long)&_ebss - (unsigned long)&_sbss;
    unsigned long staticSize = dataSize + bssSize; // Total static/global variables

    // Heap region
    unsigned long heapStart = (unsigned long)&_heap_start;
    unsigned long heapEnd = (unsigned long)&_heap_end;
    unsigned long heapSize = heapEnd - heapStart;

    // Current heap usage (if __brkval is 0, heap hasn't been used yet)
    unsigned long heapUsed = 0;
    if (__brkval != 0)
    {
        heapUsed = (unsigned long)__brkval - heapStart;
    }
    unsigned long heapFree = heapSize - heapUsed;

    // Stack region (grows down from high memory)
    unsigned long stackPointer;
    asm volatile("mov %0, sp" : "=r"(stackPointer));
    unsigned long stackEnd = (unsigned long)&_estack;
    unsigned long stackSize = stackEnd - heapEnd; // Maximum stack space
    unsigned long stackUsed = stackEnd - stackPointer; // Approximate current usage

    // Total RAM on Teensy 4.1
    const unsigned long totalRAM = 1048576; // 1MB

    // Calculate total used and free
    unsigned long totalUsed = staticSize + heapUsed + stackUsed;
    unsigned long totalFree = totalRAM - totalUsed;

    // Calculate percentages
    float staticPercent = (staticSize * 100.0) / totalRAM;
    float heapPercent = (heapUsed * 100.0) / totalRAM;
    float stackPercent = (stackUsed * 100.0) / totalRAM;
    float usedPercent = (totalUsed * 100.0) / totalRAM;

    // Print comprehensive report
    Serial.println("======================================");
    Serial.println("=== MEMORY USAGE REPORT ===");
    Serial.println("======================================");

    Serial.print("Total RAM:          ");
    Serial.print(totalRAM);
    Serial.println(" bytes (1MB)");

    Serial.println("--------------------------------------");
    Serial.println("Static/Global (.data + .bss):");
    Serial.print("  Size:             ");
    Serial.print(staticSize);
    Serial.print(" bytes (");
    Serial.print(staticPercent, 1);
    Serial.println("%)");
    Serial.print("    .data:          ");
    Serial.print(dataSize);
    Serial.println(" bytes");
    Serial.print("    .bss:           ");
    Serial.print(bssSize);
    Serial.println(" bytes");

    Serial.println("--------------------------------------");
    Serial.println("Heap (dynamic allocations):");
    Serial.print("  Max Size:         ");
    Serial.print(heapSize);
    Serial.println(" bytes");
    Serial.print("  Used:             ");
    Serial.print(heapUsed);
    Serial.print(" bytes (");
    Serial.print(heapPercent, 1);
    Serial.println("%)");
    Serial.print("  Free:             ");
    Serial.print(heapFree);
    Serial.println(" bytes");

    Serial.println("--------------------------------------");
    Serial.println("Stack:");
    Serial.print("  Max Size:         ");
    Serial.print(stackSize);
    Serial.println(" bytes");
    Serial.print("  Used (approx):    ");
    Serial.print(stackUsed);
    Serial.print(" bytes (");
    Serial.print(stackPercent, 1);
    Serial.println("%)");
    Serial.print("  Current SP:       0x");
    Serial.println(stackPointer, HEX);

    Serial.println("--------------------------------------");
    Serial.print("TOTAL USED:         ");
    Serial.print(totalUsed);
    Serial.print(" bytes (");
    Serial.print(usedPercent, 1);
    Serial.println("%)");
    Serial.print("TOTAL FREE:         ");
    Serial.print(totalFree);
    Serial.println(" bytes");

    Serial.println("======================================");
}
