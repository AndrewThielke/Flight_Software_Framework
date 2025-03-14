import requests
import json
import os
import time


CONFIG_FILE = os.path.join(os.path.dirname(__file__), "../../program_configuration.json")
WEATHER_FILE = "../api_data/weather_conditions.json"


# SpaceX's primary launch facility
DEFAULT_LATITUDE = 28.5721  # Kennedy Space Center, FL
DEFAULT_LONGITUDE = -80.6480


def load_config():
    """
    Load configuration from program_configuration.json.
    """
    if not os.path.exists(CONFIG_FILE):
        print(f"WARNING: Config file not found at {CONFIG_FILE}. Using default location.")
        return DEFAULT_LATITUDE, DEFAULT_LONGITUDE

    try:
        with open(CONFIG_FILE, "r") as file:
            config = json.load(file)
            latitude = config.get("latitude", DEFAULT_LATITUDE)
            longitude = config.get("longitude", DEFAULT_LONGITUDE)
            return latitude, longitude
    
    except json.JSONDecodeError as e:
        print(f"ERROR: Failed to parse {CONFIG_FILE}: {e}. Using default location.")
        return DEFAULT_LATITUDE, DEFAULT_LONGITUDE



def fetch_weather(latitude: float, longitude: float):
    """
    Fetch weather data for given coordinates with retry on failure.
    """
    attempts = 3
    weather_api = f"https://api.open-meteo.com/v1/forecast?latitude={latitude}&longitude={longitude}&current=temperature_2m,wind_speed_10m"

    for attempt in range(attempts):
        
        try:
            response = requests.get(weather_api, timeout=5)
            response.raise_for_status()
            data = response.json()
            return {
                "temperature_C": data["current"]["temperature_2m"],
                "wind_speed_mps": data["current"]["wind_speed_10m"]
            }
        
        except requests.exceptions.RequestException as e:
            print(f"ERROR: Failed to fetch weather data (Attempt {attempt+1}/{attempts}): {e}")
            time.sleep(2)
   
   
    print("ERROR: Could not fetch weather data. Check API connectivity.")
    return None



def main():
    """
    Main function to fetch and store weather data.
    """
    latitude, longitude = load_config()
    print(f"Fetching weather data for coordinates: {latitude}, {longitude}...")

    weather = fetch_weather(latitude, longitude)
    if weather:
        
        os.makedirs(os.path.dirname(WEATHER_FILE), exist_ok=True)
        with open(WEATHER_FILE, "w") as file:
            json.dump(weather, file, indent=4)
        print(f"Weather data saved: {WEATHER_FILE}\n")



if __name__ == "__main__":
    main()