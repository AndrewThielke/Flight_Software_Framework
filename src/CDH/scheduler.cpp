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
Scheduler::Scheduler(CDH* cdhSystem, FlightDynamics& flightDynamics)
    : cdh(cdhSystem), telemetry(cdhSystem->getTelemetry()), dynamics(flightDynamics),
      prevAlt(0.0), prevVel(0.0), prevFuel(0.0) {

    std::cout << "========================================" << std::endl;
    std::cout << "     OpenSpaceFSW Scheduler Initialized    " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "[INFO] Press Ctrl + C to terminate safely.\n" << std::endl;

    if (!instance) {
        instance = this;
    } else {
        std::cerr << "[ERROR] Multiple Scheduler instances detected! Exiting...\n";
        exit(1);
    }     
    
    std::signal(SIGINT, Scheduler::signalHandler);
}

// ==========================================
// SIGINT Handler - Handles Proper Cleanup
// ==========================================
Scheduler* Scheduler::instance = nullptr;
volatile sig_atomic_t Scheduler::stopExecutionFlag = 0;

void Scheduler::signalHandler(int signum) {
    std::cout << "\n[WARNING] Received SIGINT (Ctrl + C) - Initiating Graceful Shutdown...\n" << std::endl;
    stopExecutionFlag = 1; 
    if (instance) { instance->stop(); }
}

/**
==========================================
    The Main Execution Loop - Flight Software
==========================================
*/
void Scheduler::run() {
    std::signal(SIGINT, Scheduler::signalHandler);

    // Just making sure the command data handler was intiialized properly prior to event starting the cycles
    if (!cdh) {
        std::cerr << "[ERROR] CDH system not initialized! Cannot start mission.\n";
        return;
    }

    // Start of the heart of the program (managing each cycle)
    std::cout << "\n\n...FLIGHT SOFTWARE IS NOW RUNNING..." << std::endl;
    telemetry.setPhase(MissionPhase::PRE_LAUNCH);
    double elapsedTime = 0.0;
    const double dt = 0.1; // Simulation step (100ms)
    int currentStage = dynamics.getCurrentStage();


    while (!stopExecutionFlag) {
        cycle++;


        if (stopExecutionFlag) {
            std::cout << "\n[INFO] Stop flag received. Shutting down...\n";
            break;
        }


        // Security Check - Intrusion Monitoring
        std::ostringstream lastTelemetry;
        lastTelemetry << "Altitude: " << std::fixed << std::setprecision(2) << prevAlt
                      << " m | Velocity: " << prevVel << " m/s | Fuel: " << prevFuel << " kg";
        security.monitor(lastTelemetry.str());


        // Update Flight Dynamics
        dynamics.update(dt);
        elapsedTime += dt;


        // Populate the TelemetryData Structure based on dynamic calculation
        TelemetryData data;
        data.altitude = dynamics.getAltitude();
        data.velocity = dynamics.getVelocity();
        data.fuel = dynamics.getFuel();
        data.thrust = dynamics.getThrust();
        data.deltaV = dynamics.getDeltaV();
        data.dragForce = dynamics.getDragForce();
        data.apoapsis = dynamics.getApoapsis();
        data.periapsis = dynamics.getPeriapsis();
        data.stage = dynamics.getCurrentStage();


        // Stage Separation Check (Before even sending the data to avoid complications)
        if (dynamics.isStageSeparation()) {
            std::cout << "[SCHEDULER] Stage " << currentStage << " depleted, transitioning to next stage...\n";
            dynamics.advanceStage();
            currentStage = dynamics.getCurrentStage();
        }


        // Send Data to CDH
        std::cout << "[SCHEDULER] Confirming CDH is valid prior to telemetry processing...\n";
        if (!cdh) {
            std::cerr << "[SCHEDULER ERROR] CDH instance is NULL!!!\n";
            exit(1);
        }
        std::cout << "[SCHEDULER] CDH is valid, sending telemetry...\n";
        cdh->processTelemetry(data);


        // Update Telemetry Subsystem
        telemetry.update(data.altitude, data.velocity, data.fuel);
        telemetry.logData(data);


        // Console Output (Real-Time Updates)
        std::ostringstream output;
        output << "\nCycle: " << cycle << "\n"
               << "Time: " << elapsedTime << "s | Phase: " << telemetry.phaseToString(telemetry.getPhase()) << "\n"
               << "Altitude: " << data.altitude << " m | Velocity: " << data.velocity << " m/s | Fuel: " << data.fuel << " kg\n"
               << "Thrust: " << data.thrust << " N | Delta-V: " << data.deltaV << " m/s | Drag: " << data.dragForce << " N\n"
               << "Apoapsis: " << data.apoapsis << " m | Periapsis: " << data.periapsis << " m | Stage: " << data.stage << "\n"
               << "ADCS: Stabilizing Attitude... | GNC: Processing Navigation Data...\n";
        std::cout << output.str();


        // store these for monitoring (a feature that requires additional implementation (currently a rough prototype))
        prevAlt = data.altitude;
        prevVel = data.velocity;
        prevFuel = data.fuel;


        // Sleep for loop timing (but check flag first)
        for (int i = 0; i < 10; ++i) {
            if (stopExecutionFlag) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    std::cout << "\n[INFO] Flight Software Terminated Safely.\n" << std::endl;
}



/**
==========================================
    Scheduler Phase Update(s)
==========================================
*/
void Scheduler::updateSchedulerPhase(MissionPhase newPhase) {
    telemetry.setPhase(newPhase);
}


/**
==========================================
    Stop Method - A Graceful Shutdown
==========================================
*/
void Scheduler::stop() {
    std::cout << "[INFO] Scheduler is shutting down...\n";    
    stopExecutionFlag = 1;
    std::cout << "[INFO] Finalizing subsystems and cleaning up memory...\n";
    // need to incorporate logic here to close and flush the logger for memory management
    std::cout << "[INFO] Flight Software Terminated Safely.\n";
    std::exit(0);
}
