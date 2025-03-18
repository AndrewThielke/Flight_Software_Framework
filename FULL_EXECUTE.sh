#!/bin/bash

##################################################
### OpenSpaceFSW Full Build & Execution Script ###
##################################################

LOG_FILE="execution_log.txt"
CONFIG_FILE="$(pwd)/program_configuration.json"
ROCKET_SPECS_FILE="$(pwd)/scripts/api_data/rocket_specs.json"
WEATHER_FILE="$(pwd)/scripts/api_data/weather_conditions.json"

echo "=======================================" | tee $LOG_FILE
echo " OpenSpaceFSW Build & Execution " | tee -a $LOG_FILE
echo "=======================================" | tee -a $LOG_FILE

echo -e "\n...Checking system dependencies..." | tee -a $LOG_FILE

# Make sure that g++ is installed - PLACEHOLDER: Need to incorporate logic to install if not found
if ! command -v g++ &> /dev/null; then
    echo "ERROR: g++ not found. Install it using: sudo apt install g++ or brew install gcc." | tee -a $LOG_FILE
    exit 1
fi

# Make sure that jsoncpp is installed
if ! pkg-config --exists jsoncpp; then
    echo "ERROR: jsoncpp not found. Installing now..." | tee -a $LOG_FILE
    if [[ "$OSTYPE" == "darwin"* ]]; then
        brew install jsoncpp
    else
        sudo apt-get install libjsoncpp-dev -y
    fi
fi

# Make sure that Python3 is installed - PLACEHOLDER: Need to incorporate logic to install if not found
if ! command -v python3 &> /dev/null; then
    echo "ERROR: Python3 not found. Install it before proceeding." | tee -a $LOG_FILE
    exit 1
fi

# Getting the Rocket Specs
echo -e "\n...Pulling Data from Credible APIs..." | tee -a $LOG_FILE
echo "Fetching Rocket Specs..." | tee -a $LOG_FILE
python3 "$(pwd)/scripts/rocket_specs/get_rocket_specs.py"

if [ ! -f "$ROCKET_SPECS_FILE" ] || ! grep -q "mass_kg" "$ROCKET_SPECS_FILE"; then
    echo "ERROR: Failed to fetch rocket specs. Check API connection or script." | tee -a $LOG_FILE
    exit 1
fi

# Getting the Weather Data
echo "Fetching Weather Data..." | tee -a $LOG_FILE
python3 "$(pwd)/scripts/weather/get_weather_data.py"

if [ ! -f "$WEATHER_FILE" ] || ! grep -q "temperature_C" "$WEATHER_FILE"; then
    echo "ERROR: Failed to fetch weather data. Check API connection or script." | tee -a $LOG_FILE
    exit 1
fi

# Execution Confirmation Message
echo "...All checks passed | Ready to execute OpenSpaceFSW..." | tee -a $LOG_FILE
read -p "Type 'START_MISSION' to begin simulation: " user_input


# Compare input as-is
if [ "$user_input" != "START_MISSION" ]; then
    echo -e "\nAborting execution. Type 'START_MISSION' next time to run." | tee -a $LOG_FILE
    exit 1
fi


# Compile the project
echo -e "\nCompiling OpenSpaceFSW..." | tee -a $LOG_FILE



# Fetch library paths
JSONCPP_PATH=$(brew --prefix jsoncpp)
OPENSSL_PATH=$(brew --prefix openssl@3)

# Ensure correct environment paths
export CPATH="$JSONCPP_PATH/include:$CPATH"
export LIBRARY_PATH="$JSONCPP_PATH/lib:$LIBRARY_PATH"

export CPATH="$OPENSSL_PATH/include:$CPATH"
export LIBRARY_PATH="$OPENSSL_PATH/lib:$LIBRARY_PATH"

# Verify if jsoncpp is correctly installed
if ! pkg-config --exists jsoncpp; then
    echo "[ERROR] jsoncpp not found. Installing now..."
    brew install jsoncpp
fi

# Fetch jsoncpp flags dynamically for portability
JSONCPP_CFLAGS=$(pkg-config --cflags jsoncpp)
JSONCPP_LDFLAGS=$(pkg-config --libs jsoncpp)

# Compile the Flight Software
g++ -g -O0 \
    -I src -I src/flight_dynamics \
    -I src/core \
    -I src/mission_phases \
    -I src/flight_dynamics \
    -I src/telemetry \
    -I src/security \
    -I src/ADCS \
    -I src/GNC \
    -I src/CDH \
    -I "$JSONCPP_PATH/include" -I "$OPENSSL_PATH/include" $JSONCPP_CFLAGS \
    -L "$JSONCPP_PATH/lib" -L "$OPENSSL_PATH/lib" -Wl,-rpath,"$OPENSSL_PATH/lib" $JSONCPP_LDFLAGS \
    -lssl -lcrypto \
    -o OpenSpaceFSW \
    src/core/main.cpp src/cdh/scheduler.cpp src/cdh/cdh.cpp src/flight_dynamics/flight_dynamics.cpp \
    src/gnc/gnc.cpp src/adcs/adcs.cpp src/security/security.cpp src/telemetry/telemetry.cpp \
    -std=c++17



# Handle compilation failure(s) - PLACEHOLDER, will build on this
if [ $? -ne 0 ]; then
    echo "ERROR: Compilation failed. Check errors and try again." | tee -a $LOG_FILE
    exit 1
fi

echo -e "Compilation successful...\n" | tee -a $LOG_FILE

# Execute the Flight Software
./OpenSpaceFSW | tee -a $LOG_FILE
