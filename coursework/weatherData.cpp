#include <iostream>
#include <cpp_httplib/httplib.h>
#include <nlohmann/json.hpp>

#include "weather.h"

using namespace std;
using namespace httplib;
using json = nlohmann::json;

std::string WEATHER_API_KEY = "930673b1a77b78194a74d33db2ecd0d8";
std::string CITY_NAME = "Simferopol";
std::string WEATHER_API_ENDPOINT = "https://api.openweathermap.org/data/2.5/weather?q=" + CITY_NAME + "&appid=" + WEATHER_API_KEY + "&lang=ru&units=metric";

json cachedWeatherData;

json getWeatherData(const std::string& city) {
    std::string weatherApiUrl = "https://api.openweathermap.org/data/2.5/weather?q=" + city + "&appid=" + WEATHER_API_KEY + "&lang=ru&units=metric";

    Client cli("api.openweathermap.org");

    auto res = cli.Get(weatherApiUrl.c_str());

    if (res && res->status == 200) {
        return json::parse(res->body);
    }
    else {
        std::cerr << "Error making GET request to " << weatherApiUrl << ", status code: " << (res ? res->status : -1) << std::endl;
        return json({});
    }
}
