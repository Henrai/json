#include <iostream>
#include <string>
#include <string_view>
#include <variant>
#include "json.h"


using namespace std;
int main() {
    string_view str = 
    R"JSON({
    "aa": "BB\n",
    "cc": {
        "a": 10,
        "B": [1,2],
        "c": null,
        "d": true,
        "e": false,
   }, 
   'ff': {'aa': 12}})JSON";
    auto [jsonObj, eaten] = Json::JSONObject::parse(str);
    // cout << str.size() <<" "<< eaten << endl;
    // Json::JSONObject obj = Json::JSONObject{jsonObj.inner};
    // cout << obj << endl;
    // auto aa = obj["aa"];
    // auto cc = obj["cc"];
    // auto B  = cc["B"];
    // auto d = cc["d"];
    // cout << aa << endl;
    // cout << cc << endl;
    // cout << d  << endl;

    // int test = cc["a"].asInt();

    // cout << test << endl;

    cout << "itearte map: " << endl;
    for(auto it: jsonObj) {
        if (holds_alternative<Json::JSONObject>(it)) {
            cout << std::get<Json::JSONObject>(it) << endl;
        } else {
            auto [key, value] = std::get<Json::JSONMapValue>(it);
            cout << key << ": " << value << endl;
        }
    }

    cout << "iterate list" << endl;
    for(auto it: jsonObj["cc"]["B"]) {
        if (holds_alternative<Json::JSONObject>(it)) {
            cout << std::get<Json::JSONObject>(it) << endl;
        } else {
            auto [key, value] = std::get<Json::JSONMapValue>(it);
            cout << key << ": " << value << endl;
        }
    } 
}