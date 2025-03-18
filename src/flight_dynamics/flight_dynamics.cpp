#include "flight_dynamics.h"
#include <iostream>
#include <cmath>
#include <algorithm>



// ==========================================
//    Aerospace Constants & Environmental Data
// ==========================================
// NASA Earth Fact Sheet: https://nssdc.gsfc.nasa.gov/planetary/factsheet/earthfact.html
static constexpr double SPEED_OF_SOUND_SEA_LEVEL = 343.0;  // Speed of sound at sea level (m/s)



// ==========================================
//    Constructor: Initializes Flight Dynamics
// ==========================================
FlightDynamics::FlightDynamics(double rocketMass, double rocketFuel, double thrustSea, double thrustVac,
                                double ispSea, double ispVac, double diameter,
                                double burnTimeFirst, double burnTimeSecond)
	: mass(rocketMass), 
	initialMass(rocketMass),
	fuel(rocketFuel),
	initialFuel(rocketFuel),
	thrustSeaLevel(thrustSea),
	thrustVacuum(thrustVac),
	ispSeaLevel(ispSea),
	ispVacuum(ispVac),
	dragArea(M_PI * std::pow(diameter / 2.0, 2)),  // formula π * r² which is for the cross-sectional area
	burnTimeFirstStage(burnTimeFirst),
	burnTimeSecondStage(burnTimeSecond),
	velocity(0.0),
	altitude(0.0),
	gravity(EARTH_GRAVITY),
	deltaV(0.0),
	dragForce(0.0),
	apoapsis(0.0),
	periapsis(0.0),
	currentStage(1)  // Starts with first stage
{
	// Dynamically compute the fuel burn rate for each of the stages
	burnRateFirstStage = fuel * 0.7 / burnTimeFirstStage;  // This assumes that the first stage burns 70% of fuel
	burnRateSecondStage = fuel * 0.3 / burnTimeSecondStage;
	burnRate = burnRateFirstStage;
}


// HELPER FUNCTIONS
// Returns aerodynamic drag coefficient based on Mach number & altitude
double FlightDynamics::getDragCoefficient(double mach, double altitude) {
	
	if (mach < 0.8) return 0.25; // Low-speed subsonic (streamlined)
	
	if (mach < 1.2) return 0.45; // Transonic region (shock waves form)
	
	if (mach < 3.0) return 0.25 - 0.05 * (mach - 1.2); // Supersonic, decreasing drag
	
	if (mach < 5.0) return 0.15; // High supersonic (Mach 3-5)
	
	if (altitude > 30000) return 0.10; // Thin atmosphere at high altitudes
	
	return 0.08; // Hypersonic drag (Mach > 5)
}

// Dynamically computes thrust based on altitude
double FlightDynamics::getDynamicThrust(double altitude) const {
    double pressureFactor = std::exp(-altitude / 7000.0);  // the atmospheric pressure falls exponentially
    return thrustSeaLevel * pressureFactor + thrustVacuum * (1 - pressureFactor);
}

// Determines if the rocket should separate to the next stage
bool FlightDynamics::isStageSeparation() const {
    return (currentStage == 1 && fuel <= 0.05 * initialFuel);
}

// Advances the rocket to the next stage
void FlightDynamics::advanceStage() {
    
	if (currentStage == 1) {
        
		std::cout << "[FLIGHT DYNAMICS] Stage 1 depleted. Switching to Stage 2...\n";
        currentStage = 2;
        burnRate = burnRateSecondStage;
        thrustSeaLevel *= 0.75;  // This is assuming that 75% thrust remains for the upper stage
        thrustVacuum *= 0.8;
        mass *= 0.4;  // This is assuming that the first stage is 40% of total mass
        velocity += 500.0; // Boost from stage sep
    }
}

// Returns the current stage of the rocket
int FlightDynamics::getCurrentStage() const {
    return currentStage;
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
	

	// If dt is zero or negative, do nothing (really just is a sanity check)
    //
    if (dt <= 0.0) {return;}


    // 1) Fuel depletion & mass reduction
    double fuelConsumed = burnRate * dt;
    mass = std::max(mass - fuelConsumed, initialMass * 0.2);
    fuel = std::max(fuel - fuelConsumed, 0.0);


	// 2) Update gravity dynamically
	gravity = GM_EARTH / std::pow(EARTH_RADIUS + altitude, 2);


	// 3) Compute thrust aslong as there is fuel in the tank
    if (fuel > 0.0) {
        thrust = getDynamicThrust(altitude);
    } else {
        thrust = 0.0;  // No fuel = no thrust
    }

	
	// 4) Compute air density & drag force
    double airDensity = AIR_DENSITY_SEA_LEVEL * std::exp(-altitude / SCALE_HEIGHT);
    double mach = velocity / SPEED_OF_SOUND_SEA_LEVEL;
    double Cd = getDragCoefficient(mach, altitude);
    dragForce = 0.5 * airDensity * velocity * velocity * Cd * dragArea;
	
	
    // 5) Compute the net force
    double weight = mass * gravity;
    double netForce = std::max(thrust - dragForce - weight, 0.1 * thrust); 
	
	
    // 6) Computes the acceleration
    double acceleration = (mass > 0) ? (netForce / mass) : 0.0;


    // 7) Update velocity & altitude using proper integration
    velocity += acceleration * dt;
    altitude += velocity * dt + 0.5 * acceleration * dt * dt;  // This makes sure that the altitude change follows physics (somewhat - working on accuracy logic)


    // 8) Clamp altitude at ground level
    if (altitude < 0) {
        altitude = 0;
        velocity = std::max(velocity, 5.0);  // This makes sure that the initial velocity for liftoff is there
    }

    
    // 9) Detect Max Q transition
    static double maxDynamicPressure = 0.0;
    double dynamicPressure = 0.5 * airDensity * velocity * velocity;
    if (dynamicPressure > maxDynamicPressure) {
        maxDynamicPressure = dynamicPressure;
    } else {
        std::cout << "[FLIGHT DYNAMICS] Max Q Passed. Entering Optimal Ascent.\n";
    }


    // 10) Computes the Apoapsis (high point) & Periapsis (low point) in orbit using Orbital Mechanics
    if (velocity > 0.0) {
        // the set up
        double specificEnergy = (velocity * velocity / 2.0) - (GM_EARTH / (EARTH_RADIUS + altitude));
        double semiMajorAxis = -GM_EARTH / (2.0 * specificEnergy);
        
        // equations
        apoapsis = (2.0 * semiMajorAxis) - (EARTH_RADIUS + altitude);
        periapsis = (2.0 * (EARTH_RADIUS + altitude)) - apoapsis;
    }


    // 11) Computes the Delta-V Using the Tsiolkovsky Rocket Equation From the NASA Rocket Science Equation page... Source to be applied [PLACEHOLDER]
    if (mass > 0 && initialMass > mass) {
        deltaV = ispSeaLevel * EARTH_GRAVITY * std::log(initialMass / mass);
    } else {
        deltaV = 0.0;
    }


 	// 12) Handle stage separation
    if (isStageSeparation()) {
        advanceStage();
    }

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