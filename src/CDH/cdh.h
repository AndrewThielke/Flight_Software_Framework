#ifndef CDH_H
#define CDH_H

#include <iostream>
#include <unordered_map>
#include "telemetry/telemetry.h"
#include "mission_phase.h"



// Forward declaration to avoid circular dependency
class Scheduler;



/**
==========================================
    Command & Data Handling (CDH)
==========================================

- Manages mission execution, telemetry processing, and command handling.
- Works as the central controller, delegating tasks to the `Scheduler`.
- Handles mission phase transitions based on telemetry data.
*/
class CDH {
private:
    Scheduler* scheduler;  // Pointer to Scheduler to prevent circular dependency
    Telemetry telemetry;
    double maxDragForce = 0;  // for tracking the maximum drag force dynamically within the mission phase transition


public:
    
    // for initialization
    CDH(Scheduler* sched = nullptr);  // This allows an optional parameter
    void setScheduler(Scheduler* sched) { scheduler = sched; } 

    // for parellel data alignment
    Telemetry& getTelemetry() { return telemetry; }  


    // Core mission execution functions
    void executeCommand(const std::string& command);
    void processTelemetry(TelemetryData& data);
    void updateMissionPhase(TelemetryData& data);
    void updatePhase(MissionPhase newPhase);
    void shutdown();

};

#endif
