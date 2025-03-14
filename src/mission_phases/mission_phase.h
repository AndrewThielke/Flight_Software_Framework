#ifndef MISSION_PHASE_H
#define MISSION_PHASE_H


// Enumeration of the launch flow (somewhat realistic)
enum class MissionPhase {
    PRE_LAUNCH,
    LIFTOFF,
    MAX_Q,
    STAGE_SEPARATION,
    UPPER_STAGE_BURN,
    ORBIT_INSERTION,
    MISSION_OPS,
    ORBITAL_ADJUSTMENTS,
    DEORBIT,
    REENTRY,
    RECOVERY,
    POST_FLIGHT
};

#endif
