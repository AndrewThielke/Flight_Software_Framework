#ifndef SCHEDULER_H
#define SCHEDULER_H


// Relative Paths of subsytem modules
#include "../ADCS/adcs.h"
#include "../GNC/gnc.h"
#include "../telemetry/telemetry.h"
#include "../security/security.h"

class Scheduler {
private:
    ADCS adcs;
    GNC gnc;
    Telemetry telemetry;
    Security security;
    int cycle;

public:
    Scheduler();
    void run();
};

#endif
