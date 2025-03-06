#include "telemetry.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>



Telemetry::Telemetry() noexcept {
    // Initialize telemetry data
    currentPhase = MissionPhase::PRE_LAUNCH;
    altitude_km = 0;
    velocity_kms = 0;
    thrust_N = 0;
}


void Telemetry::setPhase(MissionPhase phase) {
    currentPhase = phase;
}


std::string Telemetry::phaseToString(MissionPhase phase) {
    
    switch (phase) {
        case MissionPhase::PRE_LAUNCH: return "Pre-Launch";
        case MissionPhase::LAUNCH: return "Launch";
        case MissionPhase::ORBIT_INSERTION: return "Orbit Insertion";
        case MissionPhase::MISSION_OPS: return "Mission Operations";
        case MissionPhase::DEORBIT: return "Deorbit";
        case MissionPhase::BURN: return "Burn";
        default: return "Unknown";
    }

}

void Telemetry::update() {

    switch (currentPhase) {
        
        case MissionPhase::PRE_LAUNCH:
            altitude_km = 0;
            velocity_kms = 0;
            thrust_N = 0;
            break;
        
        case MissionPhase::LAUNCH:
            altitude_km += 10;  // Simulate ascent
            velocity_kms += 0.5; // Acceleration
            thrust_N = 500000;
            if (altitude_km >= 120) setPhase(MissionPhase::ORBIT_INSERTION);
            break;
        
        case MissionPhase::ORBIT_INSERTION:
            altitude_km += 5;
            velocity_kms += 0.8;
            thrust_N = 10000;
            if (altitude_km >= 400) setPhase(MissionPhase::MISSION_OPS);
            break;
        
        case MissionPhase::MISSION_OPS:
            altitude_km = 400;
            velocity_kms = 7.8;
            thrust_N = 0;
            break;
        
        case MissionPhase::DEORBIT:
            altitude_km -= 15;
            velocity_kms -= 0.5;
            thrust_N = 5000;
            if (altitude_km <= 100) setPhase(MissionPhase::BURN);
            break;
        
        case MissionPhase::BURN:
            altitude_km -= 20;
            velocity_kms -= 1.0;
            thrust_N = (thrust_N > 0) ? thrust_N - 10000 : 0;
            if (altitude_km <= 0 || velocity_kms <= 0) {
                std::cout << "Burn Complete: Spacecraft Shutdown.\n";
                exit(0);
            }
            break;
    }
}

void Telemetry::logData() {
    
    std::ofstream logFile("telemetry.log", std::ios::app);
    
    if (logFile.is_open()) {
        logFile << "Phase:\n" << phaseToString(currentPhase)
                << " | Altitude: " << altitude_km << " km"
                << " | Velocity: " << velocity_kms << " km/s"
                << " | Thrust: " << thrust_N << " N\n";
        logFile.close();
    }
    
    std::cout << "\n" << phaseToString(currentPhase) 
              << " | Altitude: " << altitude_km << " km"
              << " | Velocity: " << velocity_kms << " km/s"
              << " | Thrust: " << thrust_N << " N\n";            
            
}