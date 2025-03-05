// Main Execution Point. 
// Calls all subsystems and starts the task scheduler.

#include <iostream>

// subsystem relative path(s)
#include "../ADCS/adcs.h"
#include "../GNC/gnc.h"
#include "../core/scheduler.h"
#include "../security/security.h"
#include "../telemetry/telemetry.h"

int main() {
    std::cout << "...OpenSpaceFSW Starting..." << std::endl;
    
    // Initialize subsystems
    ADCS adcs;
    GNC gnc;
    Telemetry telemetry;
    Security security;

    adcs.initialize();
    gnc.initialize();
    telemetry.start();
    security.initialize();


    // Simulate adjustments
    gnc.adjustThrust(5.0);
    adcs.adjustOrientation(10.0, 5.0, -3.0);
    telemetry.logData();
    security.encryptTelemetry();


    // Start task scheduler (real-time execution)
    Scheduler scheduler;
    scheduler.run();

    return 0;
}