#include <iostream>
#include <cpp_httplib/httplib.h>
#include <nlohmann/json.hpp>

#include "time.h"

using namespace std;
using namespace httplib;
using json = nlohmann::json;

const std::string TIME_API_ENDPOINT = "http://worldtimeapi.org/api/timezone/Europe/Simferopol";

json cachedTimeData;

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
