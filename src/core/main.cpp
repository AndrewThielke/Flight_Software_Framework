#include "cdh.h"
#include "scheduler.h"
#include "flight_dynamics.h"
#include <iostream>
#include <fstream>



int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "    OpenSpaceFSW Flight Software Boot   " << std::endl;
    std::cout << "========================================\n" << std::endl;


    // Initialize CDH and automatically load Rocket Data - will need to make this less error-prone
    CDH cdh("scripts/api_data/rocket_specs.json");


    // Ensure Flight Dynamics is properly initialized
    if (!cdh.getFlightDynamics()) {
        std::cerr << "[ERROR] Flight Dynamics failed to initialize. Mission cannot proceed.\n";
        return 1;
    }


    // Initialize Scheduler with both CDH & FlightDynamics
    Scheduler scheduler(&cdh, *cdh.getFlightDynamics());
    cdh.setScheduler(&scheduler);


    std::cout << "[MAIN] Flight software successfully initialized!\n" << std::endl;


    // Execute the Mission
    cdh.executeCommand("START_MISSION");

    return 0;
}
