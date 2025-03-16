#include "scheduler.h"
#include "cdh.h"
#include "flight_dynamics.h"
#include "mission_phase.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <iomanip>
#include <sstream>



// ==========================================
// Constructor: Initializes Dynamics and Subsystems
// ==========================================
Scheduler::Scheduler(CDH* cdhSystem) 
: cdh(cdhSystem), telemetry(cdhSystem->getTelemetry()), dynamics(500000, 7600000, 100, 311, 5.0) {
    
    std::cout << "========================================" << std::endl;
    std::cout << "     OpenSpaceFSW Scheduler Initialized    " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "[INFO] Press Ctrl + C to terminate safely.\n" << std::endl;
    
    if (!instance) {
        instance = this;  // This makes sure that instance is set only once
    } else {
        std::cerr << "[ERROR] Multiple Scheduler instances detected! Exiting...\n";
        exit(1);
    }     
    
    // Register the signal handler
    std::signal(SIGINT, Scheduler::signalHandler);
}

// ==========================================
// ==========================================
// SIGINT Handler - This Handles a Proper Cleanup
// ==========================================
Scheduler* Scheduler::instance = nullptr;
volatile sig_atomic_t Scheduler::stopExecutionFlag = 0;  // Must be defined globally

// Starts a safe shutdown
void Scheduler::signalHandler(int signum) {
    
    std::cout << "\n[WARNING] Received SIGINT (Ctrl + C) - Initiating Graceful Shutdown...\n" << std::endl;
    
    // Makes certain that the loop exits immediately
    stopExecutionFlag = 1; 
    if (instance) {instance->stop();}
}





/**
 * The primary execution loop of the OpenSpace Flight Software.
 * Handles real-time updates to flight dynamics, telemetry, and security monitoring.
 */
void Scheduler::run() {
    std::signal(SIGINT, Scheduler::signalHandler);
    

    std::cout << "\n\n\n\n\n...FLIGHT SOFTWARE IS NOW RUNNING..." << std::endl;
    telemetry.setPhase(MissionPhase::PRE_LAUNCH);
    double elapsedTime = 0.0;
    double prevAlt = 0.0, prevVel = 0.0, prevFuel = 0.0;
    const double dt = 0.1;  // 100 m/s time steps


    while (!stopExecutionFlag) {
        cycle++; // the counter


        // Check for immediate exit every cycle
        // Because if a stop flag is received while in sleep, it won't be captured.
        // This ensures that it will be.
        if (stopExecutionFlag) {
            std::cout << "\n[INFO] Stop flag received. Shutting down...\n";
            break;  // Immediately exit the loop
        }


        // Intrusion Detection - Uses previous loop telemetry data
        std::ostringstream lastTelemetry;
        lastTelemetry << "Altitude: " << std::fixed << std::setprecision(2) << prevAlt
                      << " m | Velocity: " << prevVel << " m/s | Fuel: " << prevFuel << " kg";
        security.monitor(lastTelemetry.str());


        // Update Flight Dynamics every cycle
        dynamics.update(dt);
        elapsedTime += dt;


        // Create a telemetry data structure and populate it
        TelemetryData data;
        data.altitude = dynamics.getAltitude();
        data.velocity = dynamics.getVelocity();
        data.fuel = dynamics.getFuel();
        data.thrust = dynamics.getThrust();
        data.deltaV = dynamics.getDeltaV();
        data.dragForce = dynamics.getDragForce();
        

        // Instead of passing raw values
        // this is a data structure to pass structured telemetry data to CDH and pass of responsibility to CDH
        std::cout << "[SCHEDULER] Confirming CDH is valid prior to telemetry processing...\n";
        if (!cdh) {
            std::cerr << "[SCHEDULER ERROR] CDH instance is NULL!!!\n";
            exit(1);
        }
        else {
            std::cout << "\n[SCHEDULER] CDH is valid, sending telemetry...\n";
            cdh->processTelemetry(data);
        }

        // Updates the telemetry system
        std::cout << "\n[SCHEDULER] Returned from CDH, continuing to update the telemetry subsystem..." << std::endl;
        telemetry.update(data.altitude, data.velocity, data.fuel);
        telemetry.logData();


        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
        //       Console Output
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
        std::ostringstream output;
        output << "\nCycle: " << cycle << "\n"
            << "Time: " << elapsedTime << "s | Phase: " << telemetry.phaseToString(telemetry.getPhase()) << "\n"
            << "Altitude: " << data.altitude << " m | Velocity: " << data.velocity << " m/s | Fuel: " << data.fuel << " kg\n"
            << "Thrust: " << data.thrust << " N | Delta-V: " << data.deltaV << " m/s | Drag: " << data.dragForce << " N\n"
            << "ADCS: Stabilizing Attitude... | GNC: Processing Navigation Data...\n";
        std::cout << output.str();


        // Store telemetry for next cycle intrusion monitoring
        prevAlt = data.altitude;
        prevVel = data.velocity;
        prevFuel = data.fuel;



        // Sleep for loop timing (but check flag first)
        for (int i = 0; i < 10; ++i) {  // Sleep in small chunks
            if (stopExecutionFlag) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }



    }


    std::cout << "\n[INFO] Flight Software Terminated Safely.\n" << std::endl;


}

// This makes sure that the shared telemetry phases is always up-to-date
void Scheduler::updateSchedulerPhase(MissionPhase newPhase) {
    telemetry.setPhase(newPhase);
}





// Stop method for graceful shutdown
void Scheduler::stop() {
    std::cout << "[INFO] Scheduler is shutting down...\n";    
    stopExecutionFlag = 1;  // Set the flag to stop the loop


    // Final cleanup steps
    std::cout << "[INFO] Finalizing subsystems and cleaning up memory...\n";
    telemetry.logData(); // Makes sure that subsytem telemetry logging stops properly
    std::cout << "[INFO] Flight Software Terminated Safely.\n";


    std::exit(0); // Exit cleanly
}
