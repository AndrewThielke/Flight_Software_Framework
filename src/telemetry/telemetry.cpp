#include "telemetry.h"
#include "mission_phase.h"
#include <iomanip> // for precision formatting
#include <sstream> // for string streams

// Constructor - Initializes the telemetry system
Telemetry::Telemetry() noexcept 
    : altitude_m(0), velocity_mps(0), fuel_kg(0), thrust_N(0),
      deltaV_mps(0), dragForce_N(0), apoapsis_m(0), periapsis_m(0),
      stage(0), currentPhase(MissionPhase::PRE_LAUNCH) {

    // Open log file in append mode
    logFile.open("telemetry.log", std::ios::out | std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "[Telemetry ERROR] Unable to open log file for writing.\n";
    }
}



// Destructor - Closes log file
Telemetry::~Telemetry() {
    if (logFile.is_open()) {
        logFile.close();
    }
}



// Update telemetry values individually
void Telemetry::update(double altitude, double velocity, double fuel) {
    altitude_m = altitude;
    velocity_mps = velocity;
    fuel_kg = fuel;
}



// Update telemetry using structured data (preferred method)
void Telemetry::updateFromData(const TelemetryData& data) {
    altitude_m = data.altitude;
    velocity_mps = data.velocity;
    fuel_kg = data.fuel;
    thrust_N = data.thrust;
    deltaV_mps = data.deltaV;
    dragForce_N = data.dragForce;
    apoapsis_m = data.apoapsis;
    periapsis_m = data.periapsis;
    stage = data.stage;
}





// Logs telemetry data to file & console
void Telemetry::logData(const TelemetryData& data) {
    std::ofstream logFile("telemetry.log", std::ios::out | std::ios::app);

    if (logFile.is_open()) {
		
        logFile << "Phase: " << phaseToString(currentPhase)
                << " | Altitude: " << data.altitude << " m"
                << " | Velocity: " << data.velocity << " m/s"
                << " | Fuel: " << data.fuel << " kg"
                << " | Thrust: " << data.thrust << " N"
                << " | Delta-V: " << data.deltaV << " m/s"
                << " | Drag: " << data.dragForce << " N"
                << " | Apoapsis: " << data.apoapsis << " m"
                << " | Periapsis: " << data.periapsis << " m"
                << " | Stage: " << data.stage << "\n";

        logFile.flush();

    } else {
        std::cerr << "Error: Could not open telemetry log file for writing\n";
    }
}





// Converts the mission phase to a string
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
