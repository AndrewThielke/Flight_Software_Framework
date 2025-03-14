#ifndef TELEMETRY_H
#define TELEMETRY_H

#include "mission_phase.h"
#include <iostream>
#include <fstream>
#include <string>

class Telemetry {
private:
    double altitude_m;
    double velocity_mps;
    double thrust_N;
    double fuel_kg;
    MissionPhase currentPhase;
    std::ofstream logFile;

public:
    Telemetry() noexcept; 
    ~Telemetry();  

    void update(double altitude, double velocity, double fuel);
    void logData();
    void setPhase(MissionPhase phase) { currentPhase = phase; }
    MissionPhase getPhase() const { return currentPhase; }
    std::string phaseToString(MissionPhase phase);
};

#endif
