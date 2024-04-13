#include <iostream>
#include <fstream>
#include <string>
#include <cpp_httplib/httplib.h>
#include <nlohmann/json.hpp>

using namespace std;
using namespace httplib;
using json = nlohmann::json;

const std::string WEATHER_API_KEY = "930673b1a77b78194a74d33db2ecd0d8";
const std::string CITY_NAME = "Simferopol";
const std::string WEATHER_API_ENDPOINT = "https://api.openweathermap.org/data/2.5/weather?q=" + CITY_NAME + "&appid=" + WEATHER_API_KEY + "&lang=ru&units=metric";
const std::string TIME_API_ENDPOINT = "http://worldtimeapi.org/api/timezone/Europe/Simferopol";
const std::string TEMPLATE_FILE = "template.html";

json cachedWeatherData;
json cachedTimeData;

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

json getTimeData() {
    if (!cachedTimeData.is_null()) {
        return cachedTimeData;
    }

    Client cli("worldtimeapi.org");
    auto res = cli.Get(TIME_API_ENDPOINT.c_str());
    if (res && res->status == 200) {
        cachedTimeData = json::parse(res->body);
        return cachedTimeData;
    }
    else {
        cout << "Error making GET request to " << TIME_API_ENDPOINT << ", status code: " << (res ? res->status : -1) << endl;
        return nullptr;
    }
}

int main() {
    Server svr;

    svr.Get("/", [](const Request& req, Response& res) {
        json weatherJson = getWeatherData();
        json timeJson = getTimeData();

        if (!weatherJson.is_null() && !timeJson.is_null()) {
            double temperature = weatherJson["main"]["temp"];
            string weatherDescription = weatherJson["weather"][0]["description"];
            string weatherIcon = weatherJson["weather"][0]["icon"];
            string currentTime = timeJson["datetime"];

            ifstream file(TEMPLATE_FILE);
            if (file.is_open()) {
                string htmlTemplate((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
                file.close();

                htmlTemplate.replace(htmlTemplate.find("{weatherDescription}"), string("{weatherDescription}").length(), weatherDescription);
                htmlTemplate.replace(htmlTemplate.find("{weatherIconUrl}"), string("{weatherIconUrl}").length(), "https://openweathermap.org/img/wn/" + weatherIcon + ".png");
                htmlTemplate.replace(htmlTemplate.find("{minTemperature}"), string("{minTemperature}").length(), to_string(static_cast<int>(temperature)));
                htmlTemplate.replace(htmlTemplate.find("{maxTemperature}"), string("{maxTemperature}").length(), to_string(static_cast<int>(temperature)));

                res.set_content(htmlTemplate, "text/html");
            }
            else {
                res.set_content("Failed to open template file", "text/plain");
            }
        }
        else {
            res.set_content("Failed to fetch weather or time data", "text/plain");
        }
        });

    svr.Get("/raw", [](const Request& req, Response& res) {
        json weatherJson = getWeatherData();
        json timeJson = getTimeData();

        if (!weatherJson.is_null() && !timeJson.is_null()) {
            double temperature = weatherJson["main"]["temp"];
            string weatherDescription = weatherJson["weather"][0]["description"];
            string weatherIcon = weatherJson["weather"][0]["icon"];
            string currentTime = timeJson["datetime"];

            json jsonResponse;
            jsonResponse["minTemperature"] = static_cast<int>(temperature);
            jsonResponse["maxTemperature"] = static_cast<int>(temperature);
            jsonResponse["weatherDescription"] = weatherDescription;
            jsonResponse["weatherIconUrl"] = "https://openweathermap.org/img/wn/" + weatherIcon + ".png";

            res.status = 200;
            res.set_content(jsonResponse.dump(), "application/json");
        }
        else {
            res.status = 500;
            res.set_content("Failed to fetch weather or time data", "text/plain");
        }
        });

    svr.listen("localhost", 3000);

    return 0;
}
