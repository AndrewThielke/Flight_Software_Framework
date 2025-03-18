#ifndef TELEMETRY_H
#define TELEMETRY_H

#include "mission_phase.h"
#include <iostream>
#include <fstream>
#include <string>

// Struct for telemetry data used across execution cycles
struct TelemetryData {
    double altitude;
    double velocity;
    double fuel;
    double thrust;
    double deltaV;
    double dragForce;
    double apoapsis = 0.0;
    double periapsis = 0.0;
    double mass;  // tracks the rocket's mass dynamically
    int stage = 0;   
};

class Telemetry {
private:
    double altitude_m;
    double velocity_mps;
    double thrust_N;
    double fuel_kg;
    double deltaV_mps;
    double dragForce_N;
    double apoapsis_m;
    double periapsis_m;
    int stage;
    
    MissionPhase currentPhase;
    std::ofstream logFile;

public:
    Telemetry() noexcept; 
    ~Telemetry();  

    void update(double altitude, double velocity, double fuel);
    void updateFromData(const TelemetryData& data); // New optimized function

    // so the terminal and log file transmitthe same data...
    void logData(const TelemetryData& data);
    void setPhase(MissionPhase phase) { currentPhase = phase; }
    MissionPhase getPhase() const { return currentPhase; }
    std::string phaseToString(MissionPhase phase);
};

#endif
