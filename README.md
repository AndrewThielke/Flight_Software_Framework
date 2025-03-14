# Flight_Software_Framework (OpenSpaceFSW)

**OpenSpaceFSW** is a flight software framework built for **realistic spacecraft simulation**, **autonomous navigation**, and **secure mission operations**. While having no physical hardware integration at the present time, this project aims to replicate the **core logic** of real rocket flight software, with modules for **Flight Dynamics**, **Telemetry**, **Security**, and current placeholders for **ADCS**, **GNC**, and **CDH**. Though, future expansions will integrate these modules fully, along with advanced features like multi-stage separation, mission phase transitions, and real-time data visualization.

The ultimate goal of this project is to leverage real-time geographic data and true rocket specs to accurately simulate rocket behavior. This is to enable the evaluation of successes or failures under various conditions to help reduce costs for amateur rocketry.

## **Key Features & Enhancements**

- **Physics-Based Flight Dynamics**  
  Computes the launch, orbital, and de-orbit phases using real-world physics models.  
  *Credit to NASA and SpaceX for reference data.*
  
- **Modular Telemetry System**  
  Logs the mission-critical data dynamically and supports external visualization tools (e.g., a web page that auto-launches upon execution).

- **Secure Avionics Software**  
  Implements encryption and intrusion detection measures to protect mission data from external threats.

- **Open & Extensible**  
  Built primarily in **C++** for real-time execution, with **Python** scripts for simulations and API integration. Emphasizes **flexibility**, **modularity**, and **security** for all flight software operations.

## **Project Goals**

1. **Develop** a fully functional, modular flight software framework for spacecraft.  
2. **Implement** the main subsystems: ADCS, GNC, Telemetry, CDH, Security.  
3. **Ensure** real-time execution with event-driven task scheduling.  
4. **Build** a secure and fault-tolerant avionics system (encryption, intrusion detection).  
5. **Integrate** a telemetry visualization dashboard (OpenMCT, Grafana) for mission ops.

## **New Mission Phases**

The software now supports a more **comprehensive rocket flight flow**. In code, these are enumerated in `mission_phase.h` and placeholders exist in `scheduler.cpp` that are currently being worked through:

1. **Pre-Launch**  
2. **Liftoff**  
3. **Max Q** (maximum aerodynamic pressure)  
4. **Stage Separation**  
5. **Upper Stage Burn**  
6. **Orbit Insertion**  
7. **Mission Operations**  
8. **Orbital Adjustments**  
9. **Deorbit**  
10. **Reentry**  
11. **Recovery**  
12. **Post-Flight**

**Note**: These transitions are currently placeholders (e.g., altitude or velocity thresholds). Future updates will refine triggers (time-based, fuel-based, etc.) to be **more realistic**.

## **Future Expansions**

- **Flight Dynamics & 3D Visualization**  
  Real-time 3D rendering of rocket motion (JSBSim, Orbiter SDK).
- **AI-Powered Autonomy**  
  Machine Learning for optimizing trajectory and fault detection.
- **Adaptive Failure Recovery**  
  Self-healing flight software to handle subsystem anomalies.

## **System Architecture**

### **Core Subsystems**

- **Attitude Determination & Control (ADCS)**: Placeholder. Will handle orientation, sensor fusion, etc.
- **Guidance, Navigation & Control (GNC)**: Placeholder. Will handle path planning, advanced maneuvers.
- **Telemetry Handling**: Data logging, secure transmission, real-time monitoring.
- **Command & Data Handling (CDH)**: Placeholder. Will process mission commands and fault detection.
- **Security**: Encryption, intrusion detection, real-time cybersecurity defenses.
- **Real-Time Execution**: Efficient task scheduling, event-driven logic.

| **Component**           | **Technology**                               |
|-------------------------|----------------------------------------------|
| **Languages**           | C++ (core FSW), Python (scripts, AI)         |
| **Flight Dynamics**     | JSBSim, Orbiter SDK                          |
| **Real-Time OS**        | FreeRTOS, RTEMS, or Linux-based simulation   |
| **Telemetry Dashboard** | OpenMCT, Grafana                             |
| **Cybersecurity**       | OpenSSL (encryption), AddressSanitizer (mem safety) |
| **Version Control**     | Git (GitHub)                                 |

## **Development Timeline**

**Phase 1: Planning & Setup (Weeks 1-2)**

- Define project scope and architecture.
- Set up GitHub repo with structured modules.
- Draft system architecture diagram.

**Phase 2: Core FSW Development (Weeks 3-6)**

- Implement placeholders for ADCS, GNC, Telemetry, CDH, Real-Time Execution.
- Establish event-driven scheduling.
- Create a basic test simulation loop.

**Phase 3: Secure Programming & Optimization (Weeks 7-9)**

- Implement encryption, intrusion detection, memory safety checks.
- Conduct fuzz/penetration testing.
- Develop self-healing fault tolerance mechanisms.

**Phase 4: Simulation & Visualization (Weeks 10-12)**

- Integrate with JSBSim for flight simulations.
- Build a telemetry dashboard (OpenMCT, Grafana).
- Test under simulated mission conditions.

**Phase 5: Deployment & Documentation (Weeks 13-14)**

- Finalize optimizations, modular improvements.
- Write developer documentation & API references.
- Publish open-source release.
- Engage with the aerospace dev community.

---

## **Getting Started**

### **Production-Ready (Future)**

1. **Clone the Repository**

   ```bash'''
   git clone https://github.com/AndrewThielke/Flight_Software_Framework.git
   cd Flight_Software_Framework

2. **Install Dependencies**

   '''bash'''
   sudo apt install build-essential cmake
   pip install numpy scipy

3. **Run a Test Simulation**

   '''bash'''
   [placeholder]

### During Development (**CURRENT**)

1. For Mac Users: Execute the FULL_EXECUTE.sh script (*other Operating Systems will be incorporated at a later time...*)
   '''bash'''
   ./FULL_EXECUTE.sh

2. Confirm a proper build occured (*see console/terminal*)

## Contributing

I welcome contributions! If you’re passionate about aerospace software, AI-driven autonomy, and space, join us. For major changes, please open an issue first to discuss what you’d like to change.

Contact: <AndrewThielkeSoftware@gmail.com>

## Acknowledgements

NASA Open Source Initiative, MIT Space Data Repository, and the aerospace engineers who helped inspire this project.

*Credit to SpaceX for public rocket data, and JSBSim for advanced flight modeling ideas.*
