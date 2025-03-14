// This is the MAIN Execution Point
// This calls the task Scheduler, which manages all subsystems.

#include <iostream>
#include "../core/scheduler.h"

int main() {

    // Start the task scheduler (handles real-time execution)
    Scheduler scheduler;
    scheduler.run();

    return 0;
}
