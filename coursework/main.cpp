#include <iostream>
#include <fstream>
#include <regex>
#include <cpp_httplib/httplib.h>
#include <nlohmann/json.hpp>

#include "time.h"
#include "weather.h"

using namespace std;
using namespace httplib;
using json = nlohmann::json;

const std::string TEMPLATE_FILE = "template.html";

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    std::regex re(from);
    str = std::regex_replace(str, re, to);
}

int main() {
    Server svr;

    svr.Get("/", [](const Request& req, Response& res) {
        std::string city = req.get_param_value("city");
        if (!city.empty()) {
            CITY_NAME = city; 
        }

        json weatherJson = getWeatherData(CITY_NAME);

        if (!weatherJson.is_null()) {
            string weatherDescription = weatherJson["weather"][0]["description"];
            double temperature = weatherJson["main"]["temp"];
            string weatherIcon = weatherJson["weather"][0]["icon"];

            ifstream file("template.html");
            if (file.is_open()) {
                string htmlTemplate((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
                file.close();

                replaceAll(htmlTemplate, "\\{CITY_NAME\\}", CITY_NAME);
                replaceAll(htmlTemplate, "\\{weatherDescription\\}", weatherDescription);
                replaceAll(htmlTemplate, "\\{minTemperature\\}", to_string(static_cast<int>(temperature)));
                replaceAll(htmlTemplate, "\\{maxTemperature\\}", to_string(static_cast<int>(temperature)));
                replaceAll(htmlTemplate, "\\{weatherIconUrl\\}", "https://openweathermap.org/img/wn/" + weatherIcon + ".png");

                res.set_content(htmlTemplate, "text/html");
            }
            else {
                res.set_content("Не удалось получить данные", "text/plain");
            }
        }
        else {
            res.set_content("Не удалось получить данные", "text/plain");
        }
        });

    svr.Get("/raw", [](const Request& req, Response& res) {
        json weatherJson = getWeatherData(CITY_NAME);
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
    cout << "Server started...";

    return 0;
}
