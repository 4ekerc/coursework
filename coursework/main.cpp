#include <iostream>
#include <fstream>
#include <cpp_httplib/httplib.h>
#include <nlohmann/json.hpp>

#include "weather.h"
#include "time.h"

using namespace std;
using namespace httplib;
using json = nlohmann::json;

const std::string TEMPLATE_FILE = "template.html";

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
