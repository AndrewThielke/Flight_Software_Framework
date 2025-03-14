#include "scheduler.h"
#include "flight_dynamics.h"
#include "mission_phase.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <iomanip>
#include <sstream>

// GLOBAL stop execution flag
bool stopExecution = false;



// a safe shutdown
void signalHandler(int signum) {
    std::cout << "\n[WARNING]Received SIGINT (Ctrl + C) - Shutting Down Gracefully...\n" << std::endl;
    stopExecution = true;
}



// ==========================================
//    Constructor: Initializes Dynamics and Subsystems
// ==========================================
Scheduler::Scheduler() : dynamics(500000, 7600000, 100, 311, 5.0) {
    std::cout << "========================================" << std::endl;
    std::cout << "     OpenSpaceFSW Driver Initialized    " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "[INFO] Press Ctrl + C to terminate safely.\n" << std::endl;
}



/**
==========================================
   Mission Phases: Contigent on Dynamic Data For Correct Mission Phase
==========================================

Updates the mission phase based on the obtained altitude, velocity, and fuel thresholds.
This keeps transitions consistent.
*/
void Scheduler::updateMissionPhase(double altitude, double velocity, double fuel) {
    switch (telemetry.getPhase()) {
        case MissionPhase::PRE_LAUNCH:
            if (altitude > 0.1) telemetry.setPhase(MissionPhase::LIFTOFF);
            break;

        case MissionPhase::LIFTOFF:
            if (velocity > 400.0) telemetry.setPhase(MissionPhase::MAX_Q);
            break;

        case MissionPhase::MAX_Q:
            if (fuel < 800.0) telemetry.setPhase(MissionPhase::STAGE_SEPARATION);
            break;

        case MissionPhase::STAGE_SEPARATION:
            telemetry.setPhase(MissionPhase::UPPER_STAGE_BURN);
            break;

        case MissionPhase::UPPER_STAGE_BURN:
            if (velocity >= 7800.0) telemetry.setPhase(MissionPhase::ORBIT_INSERTION);
            break;

        case MissionPhase::ORBIT_INSERTION:
            if (altitude >= 400.0 && velocity >= 7800.0) telemetry.setPhase(MissionPhase::MISSION_OPS);
            break;

        case MissionPhase::MISSION_OPS:
            if (fuel < 500.0) telemetry.setPhase(MissionPhase::ORBITAL_ADJUSTMENTS);
            break;

        case MissionPhase::ORBITAL_ADJUSTMENTS:
            if (fuel < 300.0) telemetry.setPhase(MissionPhase::DEORBIT);
            break;

        case MissionPhase::DEORBIT:
            if (altitude < 100.0) telemetry.setPhase(MissionPhase::REENTRY);
            break;

        case MissionPhase::REENTRY:
            if (altitude <= 0.0) telemetry.setPhase(MissionPhase::RECOVERY);
            break;

        case MissionPhase::RECOVERY:
            if (velocity < 1.0 && fuel < 50.0) telemetry.setPhase(MissionPhase::POST_FLIGHT);
            break;

        case MissionPhase::POST_FLIGHT:
            stopExecution = true;
            break;

        default:
            break;

    }
}



/**
 * The primary execution loop of the OpenSpace Flight Software.
 * Handles real-time updates to flight dynamics, telemetry, and security monitoring.
 */
void Scheduler::run() {
    signal(SIGINT, signalHandler);
    
    std::cout << "\n...FLIGHT SOFTWARE IS NOW RUNNING...\n" << std::endl;

    telemetry.setPhase(MissionPhase::PRE_LAUNCH);

    double elapsedTime = 0.0;
    const double dt = 1.0;  // Update frequency (1Hz)

    // Store previous telemetry for intrusion monitoring
    double prevAlt = 0.0, prevVel = 0.0, prevFuel = 0.0;

    while (!stopExecution) {
        cycle++;


        // Intrusion Detection - Uses previous loop telemetry data
        std::ostringstream lastTelemetry;
        lastTelemetry << "Altitude: " << std::fixed << std::setprecision(2) << prevAlt
                      << " m | Velocity: " << prevVel << " m/s | Fuel: " << prevFuel << " kg";
        security.monitor(lastTelemetry.str());


        // Update Flight Dynamics every cycle
        dynamics.update(dt);
        elapsedTime += dt;


        // Gets the updated telemetry data
        double altitude = dynamics.getAltitude();
        double velocity = dynamics.getVelocity();
        double fuel = dynamics.getFuel();
        double thrust = dynamics.getThrust();
        double deltaV = dynamics.getDeltaV();
        double dragForce = dynamics.getDragForce();


        // Updates the telemetry system
        telemetry.update(altitude, velocity, fuel);
        telemetry.logData();

        
        // Determines the current mission phase based on dynamic data
        updateMissionPhase(altitude, velocity, fuel);


        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
        //       Console Output
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
        std::ostringstream output;
        output << "\nCycle: " << cycle << "\n"
               << "Time: " << static_cast<int>(elapsedTime) << "s | Phase: " << telemetry.phaseToString(telemetry.getPhase()) << "\n"
               << "Altitude: " << std::fixed << std::setprecision(2) << altitude << " m"
               << " | Velocity: " << velocity << " m/s"
               << " | Fuel: " << fuel << " kg"
               << " | Thrust: " << thrust << " N"
               << " | DeltaV: " << deltaV << " N"
               << " | Drag Force: " << dragForce << " N\n"
               << "ADCS: Stabilizing Attitude...\n"
               << "GNC: Processing Navigation Data...\n";
        std::cout << output.str();


        // Store telemetry for next cycle intrusion monitoring
        prevAlt = altitude;
        prevVel = velocity;
        prevFuel = fuel;


        // Sleeps to maintain real-time execution
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "\n[INFO] Flight Software Terminated Safely.\n" << std::endl;
}