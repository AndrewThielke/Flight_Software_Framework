#ifndef CDH_H
#define CDH_H

#include <iostream>
#include <unordered_map>
#include "telemetry/telemetry.h"
#include "mission_phase.h"
#include "flight_dynamics.h"




// Forward declaration to avoid circular dependency
class Scheduler;

// Forward declaration to avoid circular dependency
class FlightDynamics;


/**
==========================================
    Command & Data Handling (CDH)
==========================================

- Manages mission execution, telemetry processing, and command handling.
- Works as the central controller, delegating tasks to the `Scheduler`.
- Handles mission phase transitions based on telemetry data.
- Dynamically loads rocket parameters from SpaceX API and initializes FlightDynamics.

*/
class CDH {
private:
    Scheduler* scheduler;  // Pointer to Scheduler to prevent circular dependency
    Telemetry telemetry;
    double maxDragForce = 0;  // for tracking the maximum drag force dynamically within the mission phase transition

    // SpaceX API Rocket Parameters (these are actually loaded from the JSON file that is populated by the SpaceX API)
    std::string rocketName;
    double rocketMass = 0.0;
    double fuelMass = 0.0;
    double thrustSeaLevel = 0.0;
    double thrustVacuum = 0.0;
    double ispSeaLevel = 0.0;
    double ispVacuum = 0.0;
    double diameter = 0.0;
    double burnTimeFirstStage = 0.0;
    double burnTimeSecondStage = 0.0;
    int stages = 1;
    int engineCount = 1;
    bool reusable = false;

    // The Flight Dynamics Engine
    FlightDynamics* flightDynamics = nullptr;


    // this loads rocket data from SpaceX API (and of course handles errors as they come)
    void loadRocketData(const std::string& configFile);    

public:
    
    // for initialization
    CDH(const std::string& configFile = ""); 
    void setScheduler(Scheduler* sched) { scheduler = sched; } // allows setting the scheduler even after initialization
    FlightDynamics* getFlightDynamics() { return flightDynamics; }
    void setFlightDynamics(FlightDynamics* fd) { flightDynamics = fd; }

    // for parellel data alignment and gets the telemetry data
    Telemetry& getTelemetry() { return telemetry; }  


    // Core mission execution functions
    void executeCommand(const std::string& command);
    void processTelemetry(TelemetryData& data);
    void updateMissionPhase(TelemetryData& data);
    void updatePhase(MissionPhase newPhase);
    void shutdown();

};

#endif
