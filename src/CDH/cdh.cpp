#include "cdh.h"
#include "scheduler.h"
#include <iostream>
#include <fstream>


/**
==========================================
    Constructor: Initializes the CDH System
==========================================
*/
CDH::CDH(Scheduler* sched) : scheduler(sched) {
    std::cout << "========================================" << std::endl;
    std::cout << "  Command & Data Handling (CDH) Initialized  " << std::endl;
    std::cout << "========================================\n" << std::endl;
}



/**
==========================================
    Execute Mission Commands
    ==========================================
    */
   void CDH::executeCommand(const std::string& command) {
       
       if (command == "START_MISSION") {
           std::cout << "[CDH] Initializing Flight Software...\n";
           scheduler->run();  
           
        } else if (command == "TERMINATE") {
        std::cout << "[CDH] Terminating Mission...\n";
        scheduler->stop();
        
    } else {
        std::cerr << "[CDH ERROR] Unknown command: " << command << "\n";
    }
}

/**
==========================================
    Process Telemetry Data & Determine Mission Phase
==========================================
*/
void CDH::processTelemetry(TelemetryData& data) {
    // Check if scheduler instance is valid since telemetry will technically always be false since &telemetry
    // is never a pointer, just an object reference
    if (!scheduler) {
        std::cerr << "[CDH ERROR] Scheduler instance is NULL! Something went wrong.\n";
        exit(1);
    }
    else {
        std::cout << "[CDH] Scheduler instance is valid, proceeding...";
        updateMissionPhase(data);
    }

    /// Debugging only
    // std::cout << "[CDH] Telemetry Data: "
    //           << "Altitude: " << data.altitude << " m, "
    //           << "Velocity: " << data.velocity << " m/s, "
    //           << "Fuel: " << data.fuel << " kg\n";
}






/**
==========================================
    Update Mission Phase
==========================================
*/
void CDH::updateMissionPhase(TelemetryData& data) {
    
    switch (telemetry.getPhase()) {
        case MissionPhase::PRE_LAUNCH:
            if (data.altitude > 0.1) updatePhase(MissionPhase::LIFTOFF);
            break;

        case MissionPhase::LIFTOFF:
            if (data.velocity > 400.0) updatePhase(MissionPhase::MAX_Q);
            break;

        case MissionPhase::MAX_Q:
            if (data.fuel < 800.0) updatePhase(MissionPhase::STAGE_SEPARATION);
            break;

        case MissionPhase::STAGE_SEPARATION:
            updatePhase(MissionPhase::UPPER_STAGE_BURN);
            break;

        case MissionPhase::UPPER_STAGE_BURN:
            if (data.velocity >= 7800.0) updatePhase(MissionPhase::ORBIT_INSERTION);
            break;

        case MissionPhase::ORBIT_INSERTION:
            if (data.altitude >= 400.0 && data.velocity >= 7800.0) updatePhase(MissionPhase::MISSION_OPS);
            break;

        case MissionPhase::MISSION_OPS:
            if (data.fuel < 500.0) updatePhase(MissionPhase::ORBITAL_ADJUSTMENTS);
            break;

        case MissionPhase::ORBITAL_ADJUSTMENTS:
            if (data.fuel < 300.0) updatePhase(MissionPhase::DEORBIT);
            break;

        case MissionPhase::DEORBIT:
            if (data.altitude < 100.0) updatePhase(MissionPhase::REENTRY);
            break;

        case MissionPhase::REENTRY:
            if (data.altitude <= 0.0) updatePhase(MissionPhase::RECOVERY);
            break;

        case MissionPhase::RECOVERY:
            if (data.velocity < 1.0 && data.fuel < 50.0) updatePhase(MissionPhase::POST_FLIGHT);
            break;

        case MissionPhase::POST_FLIGHT:
            shutdown();
            break;

        default:
            std::cerr << "[CDH ERROR] Invalid mission phase detected!\n";
            break;
    }
}


/**
==========================================
    Log The Mission Phase Transition
==========================================
*/
void CDH::updatePhase(MissionPhase newPhase) {
    std::cout << "\n[CDH] Transitioning to Phase: " << telemetry.phaseToString(newPhase) << "\n";
    telemetry.setPhase(newPhase);  // CDH's telemetry
    scheduler->updateSchedulerPhase(newPhase);  // Ensure Scheduler knows about the phase
}


/**
==========================================
    Shutdown The Flight Execution
==========================================
*/
void CDH::shutdown() {
    std::cout << "[CDH] Shutting down system safely...\n";
    
    if (scheduler) {
        scheduler->stop();
    }

    std::cout << "[CDH] Cleanup complete. Exiting now.\n";
}
