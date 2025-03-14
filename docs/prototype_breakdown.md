
# Existing Execution Table (to be continued...)

| __Step__ | __Phase__ | __Key Events__ |
|---------|-----------|--------------|
| 1 | __Initialization__ | Print the header & confirm the dependencies exist (g++, jsoncpp, Python3)
| 2 | __Data Retrieval__ | __API Use:__ <br> - Get __rocket specs__ (`get_rocket_specs.py`) <br> - Get __weather data__ (`get_weather_data.py`)
| 3 | __Pre-Execution Confirmation__ | - Prompt user to type `"execute"` to continue on <br> - This also converts input to lowercase just incase of case sensitivity issues <br> - Abort the execution if input is incorrect
| 4 | __Compilation__ | - Compile __OpenSpaceFSW__ with `g++` <br> - Handle __compilation errors__ if any are found
| 5 | __Execution Start__ | - Launch __OpenSpaceFSW binary__ <br> - __Scheduler initialized__ <br> - __Security system initialized__ (test encryption/decryption of telemetry)
| 6 | __Simulation Loop__ | __[For each loop cycle]:__ <br> - __Telemetry Data Updates__ <br> - Fetch real-time altitude, velocity, and fuel updates <br> - Log data for debugging <br> - __Security Monitoring__ <br> - Encrypt __live telemetry data__ <br> - Print __encrypted output__ <br> - Decrypt the data and __print verification output__ <br> - __Mission Phase Transitions__ <br> - Adjust phase dynamically based on altitude & velocity <br> - Update __burn, deorbit, or orbital insertion logic__ <br> - __System Health Checks__ <br> - Fuel check: trigger warnings when fuel is low <br> - Adjust thrust and drag values dynamically
| 7 | __Shutdown__ | - Detect `Ctrl + C` signal (`SIGINT`) <br> - Execute __safe shutdown procedure__ <br> - Print __final logs__ before exit


# What's Next After Prototype Is Complete?

## Setting Up True Mission Phase Guidelines (Done)

1. Instead of vague "Burn" implementation or "orbit insertion", incorporate the fundamental phases of a rocket launch. Make sure that each one has the proper logic guiding it to make the right executions at the right time.
2. Pre-Launch (T - Minutes to Liftoff)
3. Lift Off & Max Aerodynamic Pressure (highest structural stress)
4. Stage Separation & Booster Events
5. Upper Stage Burn & Orbit Insertion
6. On-Orbit Operations
7. Orbital Adjustments or Extended Mission (Start on the foundation for ADCS and GNC)
8. Deorbit and & Re-Entry (if returning back down to Earth)
9. Recovery & Post-Flight Operations like the landing event or splashdown. Data analysis and post-flight processing is required here.

## Refining Flight Dynamics & Physics Calulations For Each Mission Phase and Controling The Correct Margins (__Current Phase__)

- Incorporate real-world launch data from NASA, SpaceX, and other Open-Source Aerospace datasets to compare the computed telemetry values that this software produces. This increases the integrity of the data.
- Validate the altitude, velocity, and drag computations using real aerospace models. Also confirm that these are being computed in the proper sequences.
- Adjust the environmental factors like atmospheric drag or graviational variation for different launch locations.
- Create PROPER default values to ensure realistic simulation outputs without manual tuning.

## Improving The Security Features

- Expand the current encryption beyond telemetry to ALL mission-critical data (i.e. system logs or execution reports)
- Create authentication & access control to prevent unauthorized access to mission logs (__last__)
- Enhance the cryptographic integraty checks to actually be able to detect tampering in encrypted telemetry

## Creating Foundation for Simulation Capabilities / Logging & Data Visualization

- Redesign telemetry logging to write into structured formats (csv or json (TBD))
- Create a real-time telemetry Dashboard using a single webpage that pops up once program executes and displays the data (similiar to NDO data for avionic software)
- Incorporate direct input from the web page that updates the configuration file that drives the software and an automatic kill/restart switch.

## Optimizing Code Efficiency & Modularity

- Refactor and optimize the actual functions to reduce redundant computation and proper handoffs
- Modularize security, telemetry, and flight dynamics for a much better maintainability level
- Improve the error handling and debugging capabilitites to identify issues in real time.

## Creating the Actual Simulation Capabilities

- Create multiple vehicle configurations (i.e. Falcon 9, Starship, SLS) by loading vehicle specs dynamically using the currently connected API. The SpaceX API contains images that could be replicated using OpenGL...
- Create failure scenarios like engine failures or random aerodynamic forces
- Improve mission phase transitions by dynamically adjusting burn, deorbit, and orbital insertion...

## Algorithms Pulled From The Ideal Rocket Equation, Posted By Glenn Research Center

Let us begin by considering the rocket drawing on the left of the figure. M is the instantaneous mass of the rocket, u is the velocity of the rocket, v is the velocity of the exhaust from the rocket, A is the area of the exhaust nozzle, p is the exhaust pressure and p0 is the atmospheric pressure. During a small amount of time dt a small amount of mass dm is exhausted from the rocket. This changes the mass of the rocket and the velocity of the rocket, and we can evaluate the change in momentum of the rocket as:

change in rocket momentum=𝑀(𝑢+d𝑢)−𝑀𝑢=𝑀d𝑢

We can also determine the change in momentum of the small mass dm that is exhausted at velocity v as:

change in exhaust momentum=d𝑚(𝑢−𝑣)−𝑢d𝑚=−𝑣d𝑚

So, the total change in momentum of the system (rocket + exhaust) is:

change in system momentum=𝑀d𝑢−𝑣d𝑚

Now consider the forces acting on the system, neglecting the drag on rocket. The weight of the rocket is M g (gravitational constant) acting at an angle a to the flight path. The pressure force is given by A(p – p0) acting in the positive u direction. Then the total force on the system is:

force on the system=𝐴(𝑝−𝑝0)−𝑀𝑔cos𝑎

The change in momentum of the system is equal to the impulse on the system which is equal to the force on the system times the change in time dt. So, we can combine the previous two equations:

𝑀d𝑢−𝑣d𝑚=[𝐴(𝑝−𝑝0)−𝑀𝑔cos𝑎]d𝑡

If we ignore the weight force, and perform a little algebra, this becomes:

𝑀d𝑢=[𝐴(𝑝−𝑝0)]d𝑡+𝑣d𝑚

Now the exhaust mass dm is equal to the mass flow rate mdot (𝐦˙) times the increment of time dt. So, we can write the last equation as:

𝑀d𝑢=[𝐴(𝑝−𝑝0)+𝑚˙𝑣]d𝑡

On the web page describing the specific impulse, we introduce the equivalent exit velocity Veq which is defined as:

𝑉𝑒𝑞=𝑣+𝐴(𝑝−𝑝0)𝑚˙

If we substitute the value of Veq into the momentum equation we have:

𝑀d𝑢=𝑉𝑒𝑞𝑚˙d𝑡

The 𝐦˙𝐝𝐭 is the amount of change of the instantaneous mass of the rocket. The sign of this term is negative because the rocket is losing mass as the propellants are exhausted.

𝑚˙d𝑡=−d𝑀

Substituting into the momentum equation:

𝑀d𝑢=−𝑉𝑒𝑞d𝑀

d𝑢=−𝑉𝑒𝑞d𝑀𝑀

We can now integrate this equation:

Δ𝑢=−𝑉𝑒𝑞ln𝑀

where delta u (𝚫𝐮) represents the change in velocity, and ln is the symbol for the natural logarithmic function. The limits of integration are from the initial mass of the rocket to the final mass of the rocket. The instantaneous mass of the rocket M, the mass is composed of two main parts, the empty mass me and the propellant mass mp.

The empty mass does not change with time, but the mass of propellants on board the rocket does change with time:

𝑀(𝑡)=𝑚𝑒+𝑚𝑝(𝑡)

Initially, the full mass of the rocket mf contains the empty mass and all of the propellant at lift off. At the end of the burn, the mass of the rocket contains only the empty mass:

𝑀𝑖𝑛𝑖𝑡𝑖𝑎𝑙=𝑚𝑓=𝑚𝑒+𝑚𝑝

𝑀𝑓𝑖𝑛𝑎𝑙=𝑚𝑒

Substituting for these values we obtain:

Δ𝑢=𝑉𝑒𝑞ln(𝑚𝑓𝑚𝑒)

This equation is called the ideal rocket equation. There are several additional forms of this equation which we list here:

Using the definition of the propellant mass ratio MR

MR=𝑚𝑓𝑚𝑒

Δ𝑢=𝑉𝑒𝑞ln(MR)

Veq is related to the specific impulse Isp:

𝑉𝑒𝑞=Isp⋅𝑔0

where g0 is the gravitational constant. So the change in velocity can be written in terms of the specific impulse of the engine:

Δ𝑢=Isp⋅𝑔0⋅ln(MR)

If we have a desired delta u for a maneuver, we can invert this equation to determine the amount of propellant required:

MR=𝑒(Δ𝑢Isp⋅𝑔0)

where e is the exponential function.

What good is all this math? Let’s check the results of the equation with a few numbers. From our discussion of specific impulse and as shown in the thrust simulator, a reasonable value for specific impulse for a liquid hydrogen/liquid oxygen rocket (like the Space Shuttle main engine) would be about 350 seconds. The delta u needed to get to a 200 mile high orbit is given by the circular orbit simulator as about 17,000 mph (~25,000 feet/sec). The gravitational constant for Earth is 32.2 feet/second squared.

Putting these values into the ideal rocket equation, the resulting mass flow ratio MR is equal to 10. From the ideal rocket equation, 90% of the weight of a rocket going to orbit is propellant weight. The remaining 10% of the weight includes structure, engines, and payload. So given the current state-of-the-art, the payload accounts for only about 1% of the weight of an ideal rocket at launch. Rockets are terribly inefficient and expensive.

If you include the effects of gravity in the rocket equation, the equation becomes:

Δ𝑢=𝑉𝑒𝑞ln(MR)−𝑔0⋅tb

where tb is the time for the burn.
