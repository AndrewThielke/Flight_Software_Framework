// Scheduler handles how the project/subsystems execute and at what schedule/fixed rate.
// It is responsible for running the main loop of the flight software.
// When the time comes to convert this to event-driven, this will need to be modified and/or disabled.

#include "scheduler.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
bool stopExecution = false;



Scheduler::Scheduler() {
    std::cout << "\n...Scheduler Initialized...\n" << std::endl;
}


// Handle Execution Kills Without Conflict
void signalHandler(int signum) {
    std::cout << "\nReceived SIGINT (Ctrl + C) - Shutting Down Gracefully..." << std::endl;
    stopExecution = true;
}


void Scheduler::run() {
    signal(SIGINT, signalHandler); // Register the signal handler
    std::cout << "\n\n\n\nFlight Software Running... (Press Ctrl + C to Quit)" << std::endl;

    while (!stopExecution) {
        cycle++;

        // Execute the telemetry state transitions/updates
        telemetry.update();
        telemetry.logData();

        // Making sure the subsystems actually execute
        adcs.update();
        gnc.update();
        security.monitor();

        // Trigger the mission phase change(s) dependant on the cycle count during iteration
        if (cycle == 5) telemetry.setPhase(MissionPhase::LAUNCH);
        if (cycle == 15) telemetry.setPhase(MissionPhase::ORBIT_INSERTION);
        if (cycle == 30) telemetry.setPhase(MissionPhase::MISSION_OPS);
        if (cycle == 50) telemetry.setPhase(MissionPhase::DEORBIT);
        if (cycle == 70) telemetry.setPhase(MissionPhase::BURN);

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "Flight Software Terminated Safely." << std::endl;
}