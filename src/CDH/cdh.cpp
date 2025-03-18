#include "flight_dynamics.h"
#include "scheduler.h"
#include "cdh.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <json/json.h>

/**
==========================================
    Constructor: Initializes the CDH System
==========================================
*/
CDH::CDH(const std::string& configFile) : scheduler(nullptr) {
    std::cout << "========================================" << std::endl;
    std::cout << "  Command & Data Handling (CDH) Initialized  " << std::endl;
    std::cout << "========================================\n" << std::endl;

    if (!configFile.empty()) {
        loadRocketData(configFile);
    }
}



/**
==========================================
    Loads the Rocket Data from SpaceX API Data File (without using a external library)
==========================================
*/
/**
 * @brief Loads Rocket Data from JSON file into Flight Dynamics
 */
void CDH::loadRocketData(const std::string& configFile) {
    std::ifstream file(configFile);
    if (!file.is_open()) {
        std::cerr << "[CDH ERROR] Unable to open SpaceX API config file: " << configFile << std::endl;
        return;
    }


    Json::Value root;
    file >> root;
    file.close();


    rocketName = root["name"].asString();
    double rocketMass = root["mass_kg"].asDouble();
    double fuelMass = root["fuel_kg"].asDouble();
    double thrustSea = root["thrust_N"].asDouble();
    double thrustVac = root["thrust_vacuum_N"].asDouble();
    double ispSea = root["ISP_sea_level"].asDouble();
    double ispVac = root["ISP_vacuum"].asDouble();
    double diameter = root["diameter_m"].asDouble();
    double burnTimeFirst = root["burn_time_sec"]["first_stage"].asDouble();
    double burnTimeSecond = root["burn_time_sec"]["second_stage"].asDouble();


    // Validate the data
    if (rocketMass <= 0 || fuelMass <= 0 || thrustSea <= 0 || thrustVac <= 0 ||
        ispSea <= 0 || ispVac <= 0 || diameter <= 0 || burnTimeFirst <= 0 || burnTimeSecond <= 0) {
        std::cerr << "[CDH ERROR] Invalid rocket data. Mission cannot proceed." << std::endl;
        return;
    }


    std::cout << "[CDH] Loaded Rocket: " << rocketName << std::endl;
    std::cout << "[CDH] Mass: " << rocketMass << " kg | Fuel: " << fuelMass << " kg" << std::endl;
    std::cout << "[CDH] Thrust: " << thrustSea << " N (SL) | " << thrustVac << " N (Vacuum)\n" << std::endl;


    // Initialize Flight Dynamics with validated data
    flightDynamics = new FlightDynamics(rocketMass, fuelMass, thrustSea, thrustVac,
                                        ispSea, ispVac, diameter, burnTimeFirst, burnTimeSecond);
}



/**
==========================================
    Execute Mission Commands
    ==========================================
    */
   void CDH::executeCommand(const std::string& command) {
       
       if (command == "START_MISSION") {
           std::cout << "[CDH] Initializing the Flight Software...\n";
           scheduler->run();  
           
        } else if (command == "TERMINATE") {
        std::cout << "[CDH] Terminating the Mission...\n";
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

    // pre reqs
    double requiredOrbitVelocity = sqrt(GM_EARTH / (EARTH_RADIUS + data.altitude));
    double specificOrbitalEnergy = (0.5 * data.velocity * data.velocity) - (GM_EARTH / (EARTH_RADIUS + data.altitude));
    double TWR = data.thrust / (data.fuel + data.mass);
    double dynamicPressure = 0.5 * AIR_DENSITY_SEA_LEVEL * exp(-data.altitude / SCALE_HEIGHT) * data.velocity * data.velocity;


    switch (telemetry.getPhase()) {
        case MissionPhase::PRE_LAUNCH:
            // Transition to liftoff once altitude is no longer at sea level
            if (data.altitude > 0.1) updatePhase(MissionPhase::LIFTOFF);
            break;


        case MissionPhase::LIFTOFF:
            /* Lift-off occurs when thrust-to-weight ratio (TWR) is greater than 1.0 and the rocket begins moving upwards.
               Transition to `MAX_Q` when dynamic pressure (q) starts increasing.
            */
            if (dynamicPressure > maxDragForce) {
                maxDragForce = dynamicPressure;
            } else {
                updatePhase(MissionPhase::MAX_Q);
            }
            break;


        case MissionPhase::MAX_Q:
            /* Transition when the maximum aerodynamic stress occurs. 
               The stage separation should only occur when the fuel is depleted for the first stage and the altitude exceeds 70 km.
            */
            if (data.fuel <= 0.1 * data.mass && data.altitude > 70000) {
                updatePhase(MissionPhase::STAGE_SEPARATION);
            }
            break;


        case MissionPhase::STAGE_SEPARATION:
            /* Ensure that the first stage is completely out of fuel before initiating stage separation.
               This should also confirm that the altitude is high enough for staging (~70 km). 
            */
            if (data.stage == 1 && data.fuel <= 0) {
                updatePhase(MissionPhase::UPPER_STAGE_BURN);
            }
            break;


        case MissionPhase::UPPER_STAGE_BURN:
            /* Transition to `ORBIT_INSERTION` once the velocity reaches the required orbital velocity. 
               This ensures that the rocket is moving at an adequate speed to establish an orbit.
            */
            if (data.velocity >= requiredOrbitVelocity) {
                updatePhase(MissionPhase::ORBIT_INSERTION);
            }
            break;


        case MissionPhase::ORBIT_INSERTION:
            /* Ensure that the orbital parameters (apoapsis and periapsis) are stable.
               A stable orbit is defined by specific orbital energy being positive.
            */
            if (specificOrbitalEnergy > 0) {
                updatePhase(MissionPhase::MISSION_OPS);
            }
            break;


        case MissionPhase::MISSION_OPS:
            /* Monitor fuel reserves for station-keeping burns or orbital adjustments.
               Transition to `ORBITAL_ADJUSTMENTS` if fuel drops below 500 kg.
            */
            if (data.fuel < 500.0) updatePhase(MissionPhase::ORBITAL_ADJUSTMENTS);
            break;


        case MissionPhase::ORBITAL_ADJUSTMENTS:
            /* Prepare for reentry by adjusting orbit.
               Transition to `DEORBIT` when periapsis drops below 300 km.
            */
            if (data.fuel < 300.0 && data.periapsis < 300000) {
                updatePhase(MissionPhase::DEORBIT);
            }
            break;


        case MissionPhase::DEORBIT:
            /* Ensure that the spacecraft is committed to reentry.
               Transition to `REENTRY` once periapsis is below 100 km.
            */
            if (data.periapsis < 100000) {
                updatePhase(MissionPhase::REENTRY);
            }
            break;


        case MissionPhase::REENTRY:
            /* Transition to controlled descent and landing.
               This phase begins once altitude is below 100 km and velocity is less than 7500 m/s.
            */
            if (data.altitude < 100000 && data.velocity < 7500) {
                updatePhase(MissionPhase::RECOVERY);
            }
            break;


        case MissionPhase::RECOVERY:
            /* Ensure a controlled landing approach.
               The transition to `POST_FLIGHT` should only occur once velocity is below 5 m/s and altitude is near the surface.
            */
            if (data.velocity < 5.0 && data.altitude < 50.0) {
                updatePhase(MissionPhase::POST_FLIGHT);
            }
            break;


        case MissionPhase::POST_FLIGHT:
            // Safely shut down the system after landing
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
