#include "flight_dynamics.h"
#include <iostream>
#include <cmath>
#include <algorithm>



// ==========================================
//    Aerospace Constants & Environmental Data
// ==========================================
const double EARTH_GRAVITY = 9.80665;  // Standard gravity in m/s^2
const double ATMOSPHERIC_PRESSURE_SEA_LEVEL = 101325;  // Pascals (Pa)
const double AIR_DENSITY_SEA_LEVEL = 1.225;  // kg/m^3
const double DRAG_COEFFICIENT = 0.5;  // Assumed coefficient for streamlined bodies
const double REF_AREA = 10.0;  // References the cross-sectional area of rocket (m²) - This adjusts per rocket specs though


// ==========================================
//    Constructor: Initializes Flight Dynamics
// ==========================================
FlightDynamics::FlightDynamics(double m, double t, double br, double isp, double dragArea)
    : mass(m), thrust(t), burnRate(br), isp(isp), velocity(0), altitude(0), fuel(1000),
      dragArea(dragArea), gravity(EARTH_GRAVITY), deltaV(0), dragForce(0) {}



/**
==========================================
   Flight Dynamics Update (These are executed PER Cycle)
==========================================

update() - Computes new altitude, velocity, and fuel consumption per timestep.
    - Uses the Newton's Laws, Rocket Equation, and atmospheric models for accurate computations that need to align with existing
    - Efficiently handles the real-time adjustments to the mass and drag force.
 */
void FlightDynamics::update(double dt) {

    // PLACEHOLDR CALCULATIONS - Refactoring and optimization needed


    // Prevent calculations if fuel is depleted
    if (fuel == 0) {
        std::cout << "[WARNING] Out of Fuel! Engine Shutdown.\n";
        thrust = 0;  // Thrust terminates when fuel runs out
    }

    /* 
        Compute Dynamic Mass Change (Rocket Mass Reduces Over Time)
        Formula: M = M_initial - (burnRate * dt)
    */
    double massCurrent = std::max(mass - (burnRate * dt), mass * 0.1);  // Prevents division by zero

    /* 
        Compute Atmospheric Drag (Quadratic Drag Model)
        Drag = 0.5 * ρ * v² * Cd * A
    */
    double airDensity = AIR_DENSITY_SEA_LEVEL * exp(-altitude / 8500);  // Exponential decay with altitude
    double drag = 0.5 * airDensity * velocity * velocity * DRAG_COEFFICIENT * dragArea;

    /* 
        Computes the Net Force on Rocket
        Net Force = Thrust - Drag - (Mass * Gravity)
    */
    double netForce = thrust - drag - (massCurrent * gravity);

    /* 
        Computes the Acceleration
        a = F_net / m
    */
    double acceleration = netForce / massCurrent;

    /* 
        Updates the Velocity (v = v0 + a * dt)
    */
    velocity += acceleration * dt;

    /* 
        Updates the Altitude (h = h0 + vΔt + ½ a(Δt)^2)
    */
    altitude += velocity * dt + 0.5 * acceleration * dt * dt;

    /* 
        Computes the Delta-V using the Tsiolkovsky Rocket Equation
        ΔV = ISP * g * ln(M_initial / M_final)
    */
   if (massCurrent > 0 && mass > massCurrent) {
        deltaV = isp * EARTH_GRAVITY * log(mass / massCurrent);  // ✅ Store computed value
    } else {
        deltaV = 0.0;  // Prevents NaN in log function
    }

    /* 
        Fuel Consumption - Prevents negative
    */
    fuel = std::max(fuel - burnRate * dt, 0.0);

}



// ==========================================
//    Getter Functions (Telemetry Access Point(s))
// ==========================================
double FlightDynamics::getAltitude() const {
    return altitude;
}

double FlightDynamics::getVelocity() const {
    return velocity;
}

double FlightDynamics::getFuel() const {
    return fuel;
}

double FlightDynamics::getThrust() const {
    return thrust;
}

double FlightDynamics::getDeltaV() const {
    return deltaV;
}

double FlightDynamics::getDragForce() const {
    return dragForce;
}