#include "flight_dynamics.h"
#include "scheduler.h"
#include "cdh.h"
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
    Process Telemetry Data & Determine Mission Phase (PER CYCLE)
==========================================
*/
void CDH::processTelemetry(TelemetryData& data) {
    // Check if scheduler instance is valid since telemetry will technically always be false since &telemetry
    // is never a pointer, just an object reference
    if (!scheduler) {
        std::cerr << "[CDH ERROR] Scheduler instance is NULL! Something went wrong.\n";
        exit(1);
    }
    std::cout << "[CDH] Scheduler instance is valid, proceeding...";


    // Tracks the highest drag force that has been encountered thus far
    if (data.dragForce > maxDragForce) {
        maxDragForce = data.dragForce;
    }
    updateMissionPhase(data);
    

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
    double requiredOrbitVelocity = sqrt(3.986e14 / (data.altitude + 6371000));
    double TWR = data.thrust / (data.altitude + EARTH_GRAVITY);
    double dynamicPressure = 0.5 * AIR_DENSITY_SEA_LEVEL * data.velocity;


    switch (telemetry.getPhase()) {
        case MissionPhase::PRE_LAUNCH:
            if (data.altitude > 0.1) updatePhase(MissionPhase::LIFTOFF);
            break;


        case MissionPhase::LIFTOFF:
            // Detects Max Q dynamically by confirming when the drag force reaches its peak
            if (data.dragForce >= maxDragForce) {  // Transition when the maximum aerodynamic stress actually occurs
                updatePhase(MissionPhase::MAX_Q);
            }
            break;


        case MissionPhase::MAX_Q:
            // Computing the real-time TWR from flight_dynamic's data
            // if the below conditional is true, the first stage should seperate

            if (dynamicPressure > maxDragForce) {
                maxDragForce = dynamicPressure;

            } else {
                // Transition to stage separation when drag starts decreasing
                updatePhase(MissionPhase::STAGE_SEPARATION);
                break;
            }


        case MissionPhase::STAGE_SEPARATION:
            // Makes certain that when the first stage has depleted fuel and cut the thrust before igniting the second stage
            // This uses the ISP and fuel burn rate to determine when the first stage is out of fuel
            if (data.fuel <= 0) {  // Ensure the first stage is out of fuel
                updatePhase(MissionPhase::UPPER_STAGE_BURN);
            }            
            break;


        case MissionPhase::UPPER_STAGE_BURN:
            // Computes the required velocity dynamically and 
            // is using the orbital velocity equation to determine if the rocket has reached the required velocity
            // GM=3.986×10^14 (m³/s²) (Earth’s gravitational parameter)
            // and R is the altitude of the rocket plus the Earth’s radius (6371000 m)
            if (data.velocity >= requiredOrbitVelocity) {
                updatePhase(MissionPhase::ORBIT_INSERTION);
            }
            break;

        
        case MissionPhase::ORBIT_INSERTION:
            // Make sure that apoapsis & the periapsis are actually stable
            // PRIOR to transitioning to operations phase
            // apoapsis = highest altitude reached in orbit
            // periapsis = lowest altitude reached in orbit

            if (data.apoapsis >= 400000 && data.periapsis >= 400000) {  
                updatePhase(MissionPhase::MISSION_OPS);
            }
            break;


        case MissionPhase::MISSION_OPS:
            if (data.fuel < 500.0) updatePhase(MissionPhase::ORBITAL_ADJUSTMENTS);
            break;


        case MissionPhase::ORBITAL_ADJUSTMENTS:
            if (data.fuel < 300.0) updatePhase(MissionPhase::DEORBIT);
            break;


        case MissionPhase::DEORBIT:
            if (data.periapsis < 100000) {  // Transition when periapsis drops below 100 km
                updatePhase(MissionPhase::REENTRY);
            }
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
