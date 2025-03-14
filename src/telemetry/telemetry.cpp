#include "telemetry.h"
#include "mission_phase.h"
#include <iomanip> // for precision formatting
#include <sstream> // for string streams


// Constructor that initializes the telemetry system
Telemetry::Telemetry() noexcept {
	currentPhase = MissionPhase::PRE_LAUNCH;
	altitude_m = 0; // meters
	velocity_mps = 0; // m/s
	thrust_N = 0;
	fuel_kg = 0; // for fuel tracking
}


// closes the log file
Telemetry::~Telemetry() {
	if (logFile.is_open()) {
		logFile.close();
	}
}


// Update telemetry from the live flight dynamics
void Telemetry::update(double altitude, double velocity, double fuel) {
	altitude_m = altitude;
    velocity_mps = velocity; // store the velocity in m/s
	fuel_kg = fuel;
}


// Logs telemetry data to file and console
void Telemetry::logData() {
    std::ofstream logFile("telemetry.log", std::ios::out | std::ios::app);
    
	if (logFile.is_open()) {
        logFile << "Phase: " << phaseToString(currentPhase)
                << " | Altitude: " << altitude_m << " m"
                << " | Velocity: " << velocity_mps << " m/s"
                << " | Fuel: " << fuel_kg << " kg"
                << " | Thrust: " << thrust_N << " N\n";
        logFile.close();
		
    } else {
        std::cerr << "Error: Could not open telemetry log file for writing\n";
    }

}


// Converts the mission phase to a string and returns 12 distinct flight phases
std::string Telemetry::phaseToString(MissionPhase phase) {
	switch (phase) {
		case MissionPhase::PRE_LAUNCH:         return "Pre-Launch";
		case MissionPhase::LIFTOFF:            return "Liftoff";
		case MissionPhase::MAX_Q:              return "Max Q";
		case MissionPhase::STAGE_SEPARATION:   return "Stage Separation";
		case MissionPhase::UPPER_STAGE_BURN:   return "Upper Stage Burn";
		case MissionPhase::ORBIT_INSERTION:    return "Orbit Insertion";
		case MissionPhase::MISSION_OPS:        return "Mission Operations";
		case MissionPhase::ORBITAL_ADJUSTMENTS:return "Orbital Adjustments";
		case MissionPhase::DEORBIT:            return "Deorbit";
		case MissionPhase::REENTRY:            return "Re-entry";
		case MissionPhase::RECOVERY:           return "Recovery";
		case MissionPhase::POST_FLIGHT:        return "Post-Flight";
		default:                               return "Unknown";
	}
}
