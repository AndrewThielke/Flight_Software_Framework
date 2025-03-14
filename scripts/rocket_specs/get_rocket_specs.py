import requests
import json
import os
import time
from difflib import get_close_matches

# Paths
CONFIG_FILE = os.path.join(os.path.dirname(__file__), "../../program_configuration.json")
ROCKET_SPECS_PATH = "../api_data/rocket_specs.json"

# API
SPACEX_API = "https://api.spacexdata.com/v4/rockets"
DEFAULT_ROCKET = "Falcon 9"  # This is just a fallback rocket if config file is missing



def load_config():
    """
    Load configuration from program_configuration.json.
    """
    if not os.path.exists(CONFIG_FILE):
        print(f"WARNING: Config file not found at {CONFIG_FILE}. Using default rocket: {DEFAULT_ROCKET}")
        return DEFAULT_ROCKET


    try:
        with open(CONFIG_FILE, "r") as file:
            config = json.load(file)
            return config.get("rocket_name", DEFAULT_ROCKET)
    

    except json.JSONDecodeError as e:
        print(f"ERROR: Failed to parse {CONFIG_FILE}: {e}. Using default rocket: {DEFAULT_ROCKET}")
        return DEFAULT_ROCKET



def fetch_rocket_data(attempts=3):
    """
    Fetch all available rocket data from SpaceX API with retry logic.
    """
    for attempt in range(attempts):
        try:
            response = requests.get(SPACEX_API, timeout=5)
            response.raise_for_status()
            return response.json()
        
        except requests.exceptions.RequestException as e:
            print(f"ERROR: Failed to fetch rocket data (Attempt {attempt+1}/{attempts}): {e}")
            time.sleep(2)
    
    print("ERROR: Could not fetch rocket data. Check API connectivity.")
    return None



def estimate_fuel_mass(rocket_name):
    """
    Estimate fuel mass if missing from API.
    """
    estimated_fuel = {
        "Falcon 9": 385000,    # 385 tons (1st stage) + 90 tons (2nd stage)
        "Starship": 1200000,   # Approximate value
        "Saturn V": 2600000    # Approximate value
        # these are simply defaults as these are critical
        # when computing burn time and thrust
    }
    
    return estimated_fuel.get(rocket_name, "Unknown")



def get_rocket_specs(rocket_name, rockets):
    """
    Finds the rocket specs when given a name, even with fuzzy matching it "in theory" should be able to obtain the proper match.
    """
    rocket_names = [r["name"] for r in rockets]
    closest_match = get_close_matches(rocket_name, rocket_names, n=1, cutoff=0.6)

    if closest_match:
        rocket_name = closest_match[0]
        
        for rocket in rockets:
            if rocket["name"] == rocket_name:
                
                # Handle missing fuel mass
                fuel_mass = rocket["first_stage"].get("fuel_amount_tons", None)
                if fuel_mass is None:
                    fuel_mass = estimate_fuel_mass(rocket_name)

                rocket_specs = {
                    "name": rocket["name"],
                    "type": rocket["type"],
                    "height_m": rocket["height"]["meters"],
                    "diameter_m": rocket["diameter"]["meters"],
                    "mass_kg": rocket["mass"]["kg"],
                    "stages": rocket["stages"],
                    "boosters": rocket["boosters"],
                    "cost_per_launch": rocket["cost_per_launch"],
                    "success_rate_pct": rocket["success_rate_pct"],
                    "first_flight": rocket["first_flight"],
                    "company": rocket["company"],
                    "country": rocket["country"],
                    "thrust_N": rocket["engines"]["thrust_sea_level"]["kN"] * 1000,
                    "thrust_vacuum_N": rocket["engines"]["thrust_vacuum"]["kN"] * 1000,
                    "ISP_sea_level": rocket["engines"]["isp"]["sea_level"],
                    "ISP_vacuum": rocket["engines"]["isp"]["vacuum"],
                    "fuel_kg": fuel_mass * 1000 if isinstance(fuel_mass, (int, float)) else "Unknown",
                    "engine_type": rocket["engines"]["type"],
                    "engine_version": rocket["engines"]["version"],
                    "engine_count": rocket["engines"]["number"],
                    "payload_weights": {p["name"]: p["kg"] for p in rocket["payload_weights"]},
                    "landing_legs": rocket["landing_legs"]["number"],
                    "reusable": rocket["first_stage"]["reusable"],
                    "burn_time_sec": {
                        "first_stage": rocket["first_stage"]["burn_time_sec"],
                        "second_stage": rocket["second_stage"]["burn_time_sec"]
                    },
                    "flickr_images": rocket["flickr_images"],
                    "wikipedia": rocket["wikipedia"],
                    "description": rocket["description"]
                }

                # Print the data for verification if necessary
                #print(json.dumps(rocket_specs, indent=4, default=str))
                return rocket_specs

    print(f"ERROR: Rocket '{rocket_name}' not found. Closest match: {closest_match[0] if closest_match else 'None'}")
    return None



def main():
    """
    Main function to fetch and store rocket specs.
    """
    rocket_name = load_config()
    print(f"Fetching specs for rocket: {rocket_name}...")


    rockets = fetch_rocket_data()
    if not rockets: return


    rocket_specs = get_rocket_specs(rocket_name, rockets)
    if rocket_specs:
        
        os.makedirs(os.path.dirname(ROCKET_SPECS_PATH), exist_ok=True)
        with open(ROCKET_SPECS_PATH, "w") as file:
            json.dump(rocket_specs, file, indent=4)
        print(f"Rocket specs saved: {ROCKET_SPECS_PATH}")
    
    else: print("ERROR: Rocket specs not found.\nInvestigation Required.")



if __name__ == "__main__":
    main()