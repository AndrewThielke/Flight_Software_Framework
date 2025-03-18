#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "adcs.h"
#include "gnc.h"
#include "telemetry/telemetry.h"
#include "security.h"
#include "flight_dynamics.h"
#include <atomic>

// Forward declaration to prevent circular dependency
class CDH;

class Scheduler {
private:
    ADCS adcs;
    GNC gnc;
    Security security;
    FlightDynamics& dynamics;
    static Scheduler* instance; // Static instance to allow access in signal handler
    static volatile sig_atomic_t stopExecutionFlag; // flag that stops execution
    

    // counter
    int cycle = 0;
    

    // Required for Scheduler Acception
    CDH* cdh;  // Pointer to reference CDH
    Telemetry& telemetry; // Referencing CDH's telemetry instance so it doesn't need it's own
    double prevAlt, prevVel, prevFuel; // for telemetry tracking


public:
    Scheduler(CDH* cdhSystem, FlightDynamics& flightDynamics);
    void run();
    void updateSchedulerPhase(MissionPhase newPhase);
    void stop();
    static void signalHandler(int signum); // Static method for signal handling
};

#endif
