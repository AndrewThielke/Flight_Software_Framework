# Flight_Software_Framework (OpenSpaceFSW)

__OpenSpaceFSW__ is an open-source flight software framework designed for realistic spacecraft simulation, autonomous navigation, and secure mission operations.

This modular system provides critical functionality for the "spacecraft", including attitude determination and control (ADCS), guidance and navigation (GNC), telemetry handling, real-time execution, and secure avionics software, all of which is built from scratch with an emphasis on accuracy and flexibility.

# Key Features & Enhancements

* __Physics-Based Flight Dynamics__: Computes launch, orbital, and de-orbit phases using real-world physics models pulled from various APIs and reputable sources (Credit to NASA and SpaceX).
* __Real-Time Environmental Data Integration__: Uses APIs to fetch live weather conditions, gravity variations, and aerodynamic factors.
* __Modular Telemetry System__: Logs mission-critical data dynamically and supports external visualization tools (i.e. web pages that autolaunch once executued).
* __Cybersecurity Measures__: Implements secure avionics software to protect against external threats (alert system).
* __Open and Extensible__: This is built using C++ for real-time execution in addition to Python for scripting, simulations, and API integration. __OpenSpaceFSW__ is highly modular, extensible, and designed with aerospace security in mind.

# Project Goals

1. Develop a fully functional modular flight software framework for spacecraft.
2. Implement ADCS, GNC, Telemetry, CDH (Command & Data Handling), and Security subsystems.
3. Ensure real-time execution with event-driven task scheduling.
4. Build a secure and fault-tolerant avionics system, incorporating encryption and intrusion detection.
5. Integrate a telemetry visualization dashboard (OpenMCT, Grafana) for mission operations.

# Future Expansions

* Flight Dynamics & 3D Visualization (Real-Time). Using JSBSim, which simulates spacecraft and aircraft movement using physics-based modeling (Credit To JSBSim-Team). JSBSim reads attitude, velocity, and control inputs from this Flight_Software_Framework. This way, the user of this application is able to see the spacecraft progress from pre-launch, launch, orbital insertion, mission operations, de-orbit, and end of life (i.e. landing again).*
* AI-powered autonomous spacecraft operations (Machine Learning-based optimization).
* Adaptive failure recovery mechanisms for self-healing flight software.

# System Architecture

## Core Subsystems

* __Attitude Determination & Control (ADCS)__: Quaternion-based stabilization, sensor fusion (Kalman Filter).
* __Guidance, Navigation & Control (GNC)__: Autonomous trajectory planning and maneuvering.
* __Telemetry Handling__: Data logging, secure transmission, and real-time monitoring.
* __Command & Data Handling (CDH)__: Processing mission commands and detecting faults.
* __Security__: Implementing encryption, intrusion detection, and real-time cybersecurity defenses.
* __Real-Time Execution__: Efficient task scheduling, event-driven flight logic.

| Component  | Technology
| -----------|-----------
| Languages     | C++ (core FSW), Python (simulations, AI)
| Flight Dynamics    | JSBSIM, Orbiter SDK
| Real-Time OS | FreeRTOS, RTEMS, or Linux-based simulation
| Telemtry Dashboard | OpenMCT
| Cybersecurity | OpenSSL (encryption), AddressSanitizer (memory safety)
| Version Control | Git (Github)

# Development Timeline

__Phase 1: Planning & Setup (Weeks 1-2)__

* Define project scope and system architecture.
* Set up GitHub repository with structured modules.
* Create initial system architecture diagram.

__Phase 2: Core FSW Development (Weeks 3-6)__

* Implement ADCS, GNC, Telemetry, CDH, and Real-Time Execution Engine.
* Establish event-driven task scheduling.
* Develop a basic test simulation loop.

__Phase 3: Secure Programming & Optimization (Weeks 7-9)__

* Implement secure avionics software (encryption, intrusion detection, memory safety).
* Conduct fuzz testing and penetration testing.
* Develop self-healing fault tolerance mechanisms.

__Phase 4: Simulation & Visualization (Weeks 10-12)__

* Integrate with JSBSim for flight simulations.
* Build a telemetry visualization dashboard (OpenMCT, Grafana).
* Test software under simulated mission conditions.

__Phase 5: Deployment & Documentation (Weeks 13-14)__

* Finalize code optimizations and modularity improvements.
* Write detailed developer documentation & API references.
* Publish open-source release on GitHub.
* Engage with the aerospace developer community.

# Getting Started

## When Production Is Ready

1. Clone the Repository

'''
    git clone <https://github.com/AndrewThielke/Flight_Software_Framework.git>
    cd Flight_Software_Framework
'''

2. Install Dependancies

'''
    sudo apt install build-essential cmake
    pip install numpy scipy
'''

3. Run a Test Simulation

'''
    python sim/test_simulation.py
'''

## During Development

1. Execute the following to Build the Project. This will also tell you the issue(s) you create during development.

'''
    g++ -I src -I /opt/homebrew/opt/openssl/include \
    -L /opt/homebrew/opt/openssl/lib \
    -o OpenSpaceFSW \
    src/core/main.cpp src/core/scheduler.cpp src/adcs/adcs.cpp src/gnc/gnc.cpp src/telemetry/telemetry.cpp src/security/security.cpp \
    -std=c++17 -lssl -lcrypto
'''

2. Run the program

'''
    ./OpenSpaceFSW
'''

3. Should see the below for confirmation of a proper build:
![Image_of_Example_Output](docs/project_images/first_output_example.jpg)


## OR
Just __execute__ this: FULL_EXECUTE.sh
*OpenSpaceFSW Full Build & Execution Script*


# Contributing

I welcome contributions and would appreciate any constructive criticism! If you're passionate about aerospace software, AI-driven autonomy, or cybersecurity, join me! Reach out to me directly at <andrewthielkesoftware@gmail.com>, I'd love to hear from other developers.

# Acknowledgements

Special thanks to NASA Open Source Initiative, MIT Space Data Repository, and aerospace engineers that I've worked with for inspiring this project.


OpenGL