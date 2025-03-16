#include <iostream>
#include "cdh.h"
#include "scheduler.h"

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "    OpenSpaceFSW Flight Software Boot   " << std::endl;
    std::cout << "========================================\n" << std::endl;


    // Make sure the initialization order is proper
    CDH cdh(nullptr);        // Creating the CDH first, without passing the Scheduler quite yet
    Scheduler scheduler(&cdh);  // Then we create the Scheduler are giving it a valid CDH
    cdh.setScheduler(&scheduler);  // Finally we set the scheduler reference inside the CDH software

    // Execute mission command
    cdh.executeCommand("START_MISSION");

    return 0;
}
