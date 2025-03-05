#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <string>

class Telemetry {
public:
    void start();
    void logData();
    std::string getTelemetry();
};

#endif
