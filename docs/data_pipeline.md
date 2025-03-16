# API Data Flow

### Data Pipeline & CDH Implications

    - Step 1: Fetch API Data
        * Use Python scripts (scripts/api_data/) to get real-time weather, launch parameters, and rocket specs.       
        * Save API response into program_configuration.json.
    
    - Step 2: Validate JSON in CDH
        * Before flight execution, CDH::loadMissionParameters() ensures mission data is valid.
        * If the data is missing or incorrect, CDH aborts mission execution.
    
    - Step 3: Scheduler Retrieves CDH Data
        * Once CDH loads the data, Scheduler reads it to ensure the correct launch parameters are used.
    
    - Step 4: Scheduler Calls Flight Dynamics
        * Once the mission starts, scheduler pulls the real-time altitude, velocity, fuel, and passes it to CDH for mission phase transitions.
    
    - Step 5: CDH Determines Mission Phases
        * CDH continuously checks flight conditions and modifies system behaviors accordingly.
