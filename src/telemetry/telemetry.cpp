#include "telemetry.h"
#include <iostream>
#include <fstream>
#include <ctime>

void Telemetry::start() {
    std::cout << "Telemetry System Online\n" << std::endl;
}

void Telemetry::logData() {
    std::ofstream logFile("telemetry.log", std::ios::app);
    if (logFile.is_open()) {
        // Generate fake telemetry data
        std::time_t now = std::time(0);
        logFile << "Timestamp: " << now << " | Altitude: 400km | Velocity: 7.8 km/s\n";
        logFile.close();
    }
    std::cout << "Telemetry Data Logged." << std::endl;
}

std::string Telemetry::getTelemetry() {
    return "Altitude: 400km, Velocity: 7.8 km/s";
}
