#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "adcs.h"
#include "gnc.h"
#include "telemetry/telemetry.h"
#include "security.h"
#include "flight_dynamics.h"

class Scheduler {
private:
    ADCS adcs;
    GNC gnc;
    Telemetry telemetry;
    Security security;
    FlightDynamics dynamics; // Flight physics calculations (in progress)

    // counter
    int cycle = 0;

public:
    Scheduler();
    void updateMissionPhase(double altitude, double velocity, double fuel);
    void run();
};

#endif
