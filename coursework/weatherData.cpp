#include <iostream>
#include <cpp_httplib/httplib.h>
#include <nlohmann/json.hpp>

#include "weather.h"

using namespace std;
using namespace httplib;
using json = nlohmann::json;

const std::string WEATHER_API_KEY = "930673b1a77b78194a74d33db2ecd0d8";
const std::string CITY_NAME = "Simferopol";
const std::string WEATHER_API_ENDPOINT = "https://api.openweathermap.org/data/2.5/weather?q=" + CITY_NAME + "&appid=" + WEATHER_API_KEY + "&lang=ru&units=metric";

json cachedWeatherData;

json getWeatherData() {
    if (!cachedWeatherData.is_null()) {
        return cachedWeatherData;
    }

    Client cli("api.openweathermap.org");
    auto res = cli.Get(WEATHER_API_ENDPOINT.c_str());
    if (res && res->status == 200) {
        cachedWeatherData = json::parse(res->body);
        return cachedWeatherData;
    }
    else {
        cout << "Error making GET request to " << WEATHER_API_ENDPOINT << ", status code: " << (res ? res->status : -1) << endl;
        return nullptr;
    }
}
