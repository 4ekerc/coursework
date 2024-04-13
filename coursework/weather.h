#pragma once

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

extern std::string WEATHER_API_KEY;
extern std::string CITY_NAME;
extern std::string WEATHER_API_ENDPOINT;

json getWeatherData(const std::string& city);
