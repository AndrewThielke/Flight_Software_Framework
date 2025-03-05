#ifndef SCHEDULER_H
#define SCHEDULER_H

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

public:
    void run();
};

#endif
