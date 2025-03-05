#include "gnc.h"
#include <iostream>
#include <cmath>

void GNC::initialize() {
    std::cout << "GNC Initialized (Guidance, Navigation & Control)" << std::endl;
}

void GNC::update() {
    std::cout << "GNC Processing Navigation Data..." << std::endl;
}

void GNC::adjustThrust(double deltaV) {
    std::cout << "Adjusting Thrust: " << deltaV << " m/s change applied." << std::endl;
}
