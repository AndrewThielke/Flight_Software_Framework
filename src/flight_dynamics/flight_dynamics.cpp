#include "flight_dynamics.h"
#include <iostream>
#include <cmath>
#include <algorithm>



// ==========================================
//    Aerospace Constants & Environmental Data
// ==========================================
// NASA Earth Fact Sheet: https://nssdc.gsfc.nasa.gov/planetary/factsheet/earthfact.html
static constexpr double SCALE_HEIGHT = 8500.0;      // Exponential scale height (m)
static constexpr double DRAG_COEFFICIENT = 0.5;     // Streamlined rocket assumption
static constexpr double REF_AREA = 10.0;            // Cross-sectional area (m²)
static constexpr double GM_EARTH = 3.986e14;        // Earth's GM (m³/s²) ~ 3.986 × 10^14


// ==========================================
//    Constructor: Initializes Flight Dynamics
// ==========================================
FlightDynamics::FlightDynamics(double m, double t, double br, double ispVal, double dArea)
    : mass(m),
      thrust(t),
      burnRate(br),
      isp(ispVal),
      velocity(0.0),
      altitude(0.0),
      fuel(1000.0),
      dragArea(dArea),
      gravity(EARTH_GRAVITY),
      deltaV(0.0),
      dragForce(0.0),
      apoapsis(0.0),
      periapsis(0.0)
{
    // No additional constructor logic needed for now.
}



/**
// ===================================================
//      update() - Main Dynamics Computation
// ===================================================
 *  Update performs a single simulation step of duration dt (seconds):
 *
 *   1) Computes the thrust from Isp, burn rate, & g₀, if fuel > 0.
 *   2) Updates the gravity based on altitude (inverse-square law).
 *   3) Computes the current rocket mass (fuel depletion).
 *   4) Computes the drag using quadratic model: 0.5 * ρ * v² * Cd * A.
 *   5) Calculates the net force = Thrust - Drag - Weight.
 *   6) Acceleration = netForce / mass.
 *   7) Updates the velocity & altitude using classical kinematics.
 *   8) If the altitude < 0, clamp to zero (rocket on ground).
 *   9) If the velocity > 0, estimate orbital parameters (apoapsis & periapsis).
 *  10) Computes the ΔV via Tsiolkovsky rocket equation.
 *  11) Subtracts the fuel usage.
*/
void FlightDynamics::update(double dt) {
	
	// 1) Thrust-Fuel logic
	// Apply Thrust only if fuel is available. | T = ISP * m_dot * g
	// No fuel equals NO thrust
	if (fuel > 0) {
		thrust = isp * burnRate * EARTH_GRAVITY; 
	} else {	
		thrust = 0;
	}
	
	
	// 2) The Dynamic Gravity (this is Altitude-Based)
    //    g(r) = g₀ * (R / (R + h))²
    {
        double radiusRatio = EARTH_RADIUS / (EARTH_RADIUS + altitude);
		double radiusRatioSq = radiusRatio * radiusRatio;
        gravity = EARTH_GRAVITY * radiusRatioSq;
    }	
	
	/* 
	3) Compute Dynamic Mass Change (Rocket Mass Reduces Over Time)
	Formula: M = M_initial - (burnRate * dt)
	While keepin at least 10% of the mass as a floor to avoid division by zero
	*/
	double massCurrent = std::max(mass - (burnRate * dt), mass * 0.1);
	
	
	/* 
	4) Compute  Drag (Quadratic Drag Model)
		ρ = ρ₀ * exp(-h / H)
		Drag = 0.5 * ρ * v² * Cd * A	
	*/
	double airDensity = AIR_DENSITY_SEA_LEVEL * std::exp(-altitude / SCALE_HEIGHT);
	double velocitySq = velocity * velocity;
	dragForce = 0.5 * airDensity * velocitySq * DRAG_COEFFICIENT * dragArea;


	/* 	
	5) Net Force
	Computes the Net Force on Rocket
		Net Force (aka F_net) = Thrust - Drag - Weight
	*/
	double weight = massCurrent * gravity;
	double netForce = thrust - dragForce - weight;


	/* 
	6) Acceleration
	Computes the Acceleration
		a = F_net / m
	*/
	double acceleration = netForce / massCurrent;


	/* 
	7) Responsible for updating the velocity and altitude
    	v = v₀ + a*dt
    	h = h₀ + v₀*dt + 0.5*a*(dt)²
	Note: Velocity shouldn't increase linearly, but exponentially.
	*/
	velocity += acceleration * dt;


    // For altitude, this is using the "v₀ dt + 0.5 a dt²" approach
    // This is applying the current velocity for the main term
    // You can choose either v₀ or v₁, but the difference is typically small 
    // when it's a small dt
	// We'll use v - 0.5 * a dt if you want "semi-implicit".)
    double altitudeChange = (velocity * dt) + (0.5 * acceleration * dt * dt);
    altitude += altitudeChange;


	// 8) Clamp the altitude to 0 if...
	// This just prevents negative altitude (Ground Collision Handling)
	if (altitude < 0) {
		altitude = 0;
		velocity = 0;  // No velocity after ground impact
	}


	// 9) Compute semi-major axis (if velocity is high enough for orbit)
    if (velocity > 0.0)
    {
        double radius = EARTH_RADIUS + altitude;  // distance from Earth's center
        double invSemiMajorAxis = (2.0 / radius) - (velocitySq / GM_EARTH);
        if (invSemiMajorAxis != 0.0) {
            double semiMajorAxis = 1.0 / invSemiMajorAxis;

            // Apoapsis & Periapsis relative to Earth's surface
            apoapsis  = (2.0 * semiMajorAxis) - radius - EARTH_RADIUS;
            periapsis = (2.0 * radius) - apoapsis - EARTH_RADIUS;
        }
    }


	/* 
	10) Computes the Delta-V using the Tsiolkovsky Rocket Equation
		ΔV = ISP * g * ln(M_initial / M_final)
	*/
	if (massCurrent > 0 && mass > massCurrent) {
		deltaV = isp * EARTH_GRAVITY * std::log(mass / massCurrent); 
	} else {
		deltaV = 0.0;  // Prevents NaN in log function
	}


	/* 
	11) Fuel Consumption - also prevents negative
	*/
	fuel = std::max(fuel - burnRate * dt, 0.0);

}



// ==========================================
//    Getter Functions (Telemetry Access Point(s))
// ==========================================
double FlightDynamics::getAltitude() const { return altitude; }
double FlightDynamics::getVelocity() const { return velocity; }
double FlightDynamics::getFuel() const { return fuel; }
double FlightDynamics::getThrust() const { return thrust; }
double FlightDynamics::getDeltaV() const { return deltaV; }
double FlightDynamics::getDragForce() const { return dragForce; }
double FlightDynamics::getApoapsis() const { return apoapsis; }
double FlightDynamics::getPeriapsis() const { return periapsis; }