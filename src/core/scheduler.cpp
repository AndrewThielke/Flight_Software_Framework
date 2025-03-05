// Scheduler handles how the project/subsystems execute and at what schedule/fixed rate.
// It is responsible for running the main loop of the flight software.
// When the time comes to convert this to event-driven, this will need to be modified and/or disabled.

#include "scheduler.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>

bool stopExecution = false;

void signalHandler(int signum) {
    std::cout << "\nReceived SIGINT (Ctrl + C) - Shutting Down Gracefully..." << std::endl;
    stopExecution = true;
}

void Scheduler::run() {
    signal(SIGINT, signalHandler); // Register signal handler
    std::cout << "\n\n\n\nFlight Software Running... (Press Ctrl + C to Quit)" << std::endl;

    while (!stopExecution) {
        adcs.update();
        gnc.update();
        telemetry.logData();
        security.monitor();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "Flight Software Terminated Safely." << std::endl;
}
