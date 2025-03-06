// Main Execution Point.
// Calls the Task Scheduler which manages all subsystems.

#include <iostream>
#include "../core/scheduler.h"

int main() {
    std::cout << "...OpenSpaceFSW Starting..." << std::endl;

    // Start task scheduler (handles real-time execution)
    Scheduler scheduler;
    scheduler.run();

    return 0;
}
