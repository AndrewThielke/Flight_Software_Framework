#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <string>

/*
Telemetry Mission Phases. 
See docs/state_transitions.xlsx
*/ 
enum class MissionPhase {
    PRE_LAUNCH,
    LAUNCH,
    ORBIT_INSERTION,
    MISSION_OPS,
    DEORBIT,
    BURN // route to 0s
};


class Telemetry {
    public:
        Telemetry() noexcept;
        void start();
        void logData();
        void update();
        void setPhase(MissionPhase phase);
        std::string getTelemetry();

    private:
        MissionPhase currentPhase;
        double altitude_km;
        double velocity_kms;
        double thrust_N;
        std::string phaseToString(MissionPhase phase);
};

#endif
